#include "xmllyrics.h"
#include <fmt/format.h>
#include <string>
#include <algorithm>

Block* XMLLyrics::blockByTiming(size_t timing) {
    if (currentBlock >= blocks.size()) {
        return nullptr;
    }
    Block& block = blocks[currentBlock];
    if(block.ca.endtime >= timing) {
        return &block;
    } else {
        currentBlock++;
        return blockByTiming(timing);
    }

    // bool take_next = false;
    // for(Block& block: blocks) {
    //     // Find perfect match
    //     // ret
    //     if(block.ca.endtime <= timing){
    //         continue;
    //     }
    //     return &block;
    //     if(block.ca.starttime <= timing + prefetch && block.ca.endtime >= timing) {
    //         return &block;
    //     }
    //
    //     if(take_next && block.ca.endtime >= timing) {
    //         return &block;
    //     }
    //
    //     // Block has endet, but next block is not yet active, set take_next flag
    //     if(block.ca.endtime <= timing) {
    //         take_next = true;
    //     }
    // }
    // return nullptr;
}

std::map<std::string, std::map<std::string, std::string>> XMLLyrics::parseTrackSettings(tinyxml2::XMLElement *e) {
    std::map<std::string, std::map<std::string, std::string>> track_settings;
    auto tracks = e->FirstChildElement("tracks");
    for(auto tsettings = tracks->FirstChildElement("tracksettings"); tsettings != nullptr; tsettings = tsettings->NextSiblingElement("tracksettings")) {
        std::map<std::string, std::string> set;
        for(auto attr = tsettings->FirstAttribute(); attr != nullptr; attr = attr->Next()) {
            set[attr->Name()] = attr->Value();
        }
        track_settings[set["name"]] = set;
    }
    return track_settings;
}

XMLLyrics::XMLLyrics(const std::string& filename)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.c_str());
    const auto base_element = doc.FirstChildElement("song");
    if(base_element == nullptr)
        throw std::runtime_error("XML Error, no karaoke tag");

    ca.load(base_element);
    const auto tca = parseTrackSettings(base_element);

    artist = CommonAttributes::getAttributeWithDefault("Unknown Artist", "artist", base_element);
    title = CommonAttributes::getAttributeWithDefault("Unknown Title", "title", base_element);
    titleTiming = std::stol(CommonAttributes::getAttributeWithDefault("2000", "titletime", base_element));
    prefetch = std::stol(CommonAttributes::getAttributeWithDefault("1000", "prefetch", base_element));
    append = std::stol(CommonAttributes::getAttributeWithDefault("3000", "append", base_element));

    if(base_element->Attribute("titlecolor")) {
        titleColor = Color(base_element->Attribute("titlecolor"));
    } else {
        titleColor = Color(0xffffffff);
    }

    if(base_element->Attribute("bordercolor")) {
        borderColor = Color(base_element->Attribute("bordercolor"));
    } else {
        borderColor = Color(0xff000000);
    }

    titleImage = renderTitle();


    size_t last_endtime = titleTiming;
    for(auto e_block = base_element->FirstChildElement("block");
        e_block != nullptr; e_block = e_block->NextSiblingElement("block")) {
        Block block(e_block, ca, tca, last_endtime);
        blocks.push_back(block);
        if(block.ca.starttime < last_endtime) {
            throw std::runtime_error("Start before end");
        }
        last_endtime = block.ca.endtime;
    }
    ca.starttime = blocks.front().ca.starttime;
    ca.endtime = blocks.back().ca.endtime + append;
}

Image XMLLyrics::renderString(Font font, Color c, std::string str) const {
    std::vector<Font::GlyphBitmap> glyphs = font.renderText(str);
    size_t height = 0;
    int width = 0;
    int baseline = 0;
    int min_offset = 0;
    int last_y_advance = 0;

    for(auto const& glyph : glyphs) {
        width += glyph.x_advance;
        height = std::max(height, glyph.height + glyph.y_offset + last_y_advance);
        min_offset = std::min(min_offset, glyph.y_offset + last_y_advance);
        last_y_advance = glyph.y_advance;
    }
    baseline = std::abs(min_offset);
    height += baseline;

    Image img(width, height);

    uint x = 0;
    uint y = 0;
    for(auto const& glyph: glyphs) {
        int y_offset = height - (baseline + glyph.y_offset) - glyph.height;

        for (size_t j = 0; j < glyph.height; j++) {
            for (size_t i = 0; i < glyph.width; i++) {
                size_t buffercord = (i+x+glyph.x_offset) + (j+y+y_offset) * width;
                size_t fontcord = i + j * glyph.width;
                c.a = glyph.data[fontcord];
                if(buffercord < img.data.size()) {
                    img.data[buffercord].alpha_blend(c);
                }
            }
        }
        x += glyph.x_advance;
        y += glyph.y_advance;
    }
    return img;
}

Image XMLLyrics::renderTitle() const {
    Font font(Font::getFontFile(ca.fontname), ca.fontsize, false);
    Image img_title = renderString(font, titleColor, title);
    Image img_artist = renderString(font, titleColor, artist);

    size_t gap = img_title.height() + img_artist.height();
    size_t width = std::max(img_title.width(), img_artist.width());
    size_t height = 2 * gap;

    Image title(width, height);
    title.fill(0x00000000);
    title.drawImage(img_artist, (width - img_artist.width()) / 2, 0);
    title.drawImage(img_title, (width - img_title.width()) / 2, img_artist.height() + gap);
    return title;
}

