#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

#include "color.h"
#include "font.h"

class Image {
public:
        Image() = default;
    Image(size_t width, size_t height);
    Image(const std::string& filename);
    Image(const Image& img) : data(img.data) /*, m_font(img.m_font) */ {
        m_width = img.m_width;
        m_height = img.m_height;
    }

    void resize(int width, int height) {
        m_height = height;
        m_width = width;
        data.resize(width * height);
    }
    inline void fill(const Color& c) {
        std::fill(data.begin(), data.end(), c);
    }
    bool load(const std::string& filename);
    void save(const std::string& filename) const;
    size_t width() const;
    size_t height() const;
    const Color* scanLine(size_t line) const;
    void drawImage(const Image& source, /*size_t sx, size_t sy, size_t sw, size_t sh,*/ size_t x = 0, size_t y = 0);
    Image scaled(double width, double height);
    //void drawRect( size_t x, size_t y, size_t w, size_t h);
    Color alpha_blend(Color c_old, Color c_new, uint8_t I) const;
    std::vector<Color> data;

private:
    size_t m_width;
    size_t m_height;
};

#endif // IMAGE_H
