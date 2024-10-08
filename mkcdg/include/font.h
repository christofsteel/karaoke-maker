#ifndef FONT_H
#define FONT_H

#include <string>
#include <fontconfig/fontconfig.h>
#include <iostream>
#include <vector>
#include <map>
//#include "freetype/ftbitmap.h"
#include "ft2build.h"
#include FT_FREETYPE_H

#include "harfbuzz/hb.h"
#include "harfbuzz/hb-ft.h"

#include "color.h"
static FT_Library* library;
static FcConfig* config;
static std::map<std::string, std::string> fontnamecache;

class Font {
public:

        struct GlyphBitmap {
                std::vector<uint8_t> data;
                size_t width;
                size_t height;
                int x_offset;
                int y_offset;
                int x_advance;
                int y_advance;
                int horiBearingY;
                Color a;
                Color b;
        };


        Font() : Font(Font::getFontFile("Sans"), 16, true) {
        }

    Font(std::string name, uint32_t size, bool antialias) {
        if (!library) {
            library = new FT_Library();
            FT_Init_FreeType(library);
        }
        this->name = name;
        this->size = size;
        int error = FT_New_Face(*library, name.c_str(), 0, &face);
        if (error) {
            std::cerr << "Could not load font " << name << std::endl;
            error = FT_New_Face(*library, getFontFile(name).c_str(), 0, &face);
            if(error)
                exit(1);
            else
                std::cerr << "Found " << getFontFile(name) << std::endl;
        }
        error = FT_Set_Pixel_Sizes(face, 0, size);
        if (error) {
            std::cerr << "Could not load pixelsize " << name << std::endl;
            exit(1);
        }
        this->antialias = antialias;
    }

    static std::string getFontFile(std::string name) {
        if(fontnamecache.find(name) != fontnamecache.end()) {
            return fontnamecache[name];
        } else {
        if(!config)
            config = FcInitLoadConfigAndFonts();

        FcPattern* pat = FcNameParse(reinterpret_cast<const FcChar8*>(name.c_str()));

        FcConfigSubstitute(config, pat, FcMatchPattern);
        FcDefaultSubstitute(pat);

        FcResult result;
        FcPattern* font  = FcFontMatch(config, pat, &result);

        std::string fontfile;

        if(font) {
            FcChar8* file = nullptr;
            if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
                fontfile = std::string(reinterpret_cast<char*>(file));
            }
            FcPatternDestroy(font);
        }
        FcPatternDestroy(pat);
        fontnamecache[name] = fontfile;
        std::cerr << "FONT " <<name << ": "<< fontfile << std::endl;
        return fontfile;
        }
    }

    inline uint32_t pointSize() const {
        return size;
    }

    inline std::string family() const {
        return name;
    }

    std::vector<Font::GlyphBitmap> renderText(const std::string& text) {

        //hb_buffer_clear_contents(hb_buf);

        hb_buffer_t *buf = hb_buffer_create();
        hb_buffer_add_utf8(buf, text.c_str(), -1, 0, -1);
        hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
        hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
        hb_buffer_set_language(buf, hb_language_from_string("en", -1));
        hb_font_t *hb_font = hb_ft_font_create(face, nullptr);

        hb_shape(hb_font, buf, nullptr, 0);

        uint glyph_count;

        hb_glyph_info_t * glyph_info  = hb_buffer_get_glyph_infos(buf, &glyph_count);
        hb_glyph_position_t * glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

        std::vector<Font::GlyphBitmap> bitmaps;
        for(uint i=0; i < glyph_count; i++) {
            bitmaps.push_back(renderGlyph(glyph_info, glyph_pos, i));
        }
        hb_font_destroy(hb_font);
        hb_buffer_destroy(buf);

        return bitmaps;
    }

    Font::GlyphBitmap renderGlyph(hb_glyph_info_t* glyph_info, hb_glyph_position_t* glyph_pos, size_t pos) {
        Font::GlyphBitmap gb;
        uint glyphid = glyph_info[pos].codepoint;
        gb.data = getGlyphBitmap(glyphid, &gb.width, &gb.height, &gb.horiBearingY);

        gb.x_offset = glyph_pos[pos].x_offset / 64;
        gb.y_offset = gb.horiBearingY - gb.height;
//        gb.y_offset = glyph_pos[pos].y_offset / 64;
        gb.x_advance = glyph_pos[pos].x_advance / 64;
        gb.y_advance = glyph_pos[pos].y_advance / 64;
        return gb;
    }



    inline uint getGlyphCount(const std::string& text) const {
        hb_buffer_t *buf = hb_buffer_create();
        hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
        hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
        hb_buffer_set_language(buf, hb_language_from_string("en", 2));

        hb_font_t *hb_font = hb_ft_font_create(face, nullptr);

        hb_buffer_add_utf8(buf, text.c_str(), text.length(), 0, text.length());
        hb_shape(hb_font, buf, NULL, 0);

        uint glyph_count;
        hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
        hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
        hb_font_destroy(hb_font);
        hb_buffer_destroy(buf);
        return glyph_count;
    }

    inline const std::vector<uint8_t> getGlyphBitmap(uint glyphIndex, size_t* width, size_t* height, int* horiBearingY) const {
        int error;
        if (antialias) {
            error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
            *width = face->glyph->bitmap.width;
            *height = face->glyph->bitmap.rows;
            *horiBearingY = face->glyph->metrics.horiBearingY / 64;
            return std::vector<uint8_t>(face->glyph->bitmap.buffer, face->glyph->bitmap.buffer + (*width * *height));
        } else {
            error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO);
            error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
            //std::vector<uint8_t> debug(face->glyph->bitmap.buffer, face->glyph->bitmap.buffer + (face->glyph->bitmap.rows * face->glyph->bitmap.pitch));
            *width = face->glyph->bitmap.width;
            *height = face->glyph->bitmap.rows;
            *horiBearingY = face->glyph->metrics.horiBearingY / 64;
            return Font::unpack_mono_bitmap(&face->glyph->bitmap);
        }
/*        if (error) {
            std::cerr << "Could not render " << c << std::endl;
            exit(1);
        }*/
    }



   static std::vector<uint8_t> unpack_mono_bitmap(FT_Bitmap* bitmap)
{
       std::vector<uint8_t> result(bitmap->rows * bitmap->width);
       for(int y = 0; y < bitmap->rows; y++) {
           for(int x = 0; x < bitmap->width; x++) {
               int bytenr = x / (8*sizeof(char));
               int bytepos = x % (8*sizeof(char));
               uint8_t byte = bitmap->buffer[y * bitmap->pitch + bytenr];
               uint8_t value = (byte & (1 << (7 - bytepos))) ? 255 : 0;
               result[y * bitmap->width + x] = value;
           }
       }
    return result;
}
    FT_Face face;
private:
    bool antialias;
    std::string name;
    uint32_t size;

};

#endif // FONT_H
