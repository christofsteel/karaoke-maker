#ifndef COLOR_H
#define COLOR_H

#include <cstdint>
#include <iostream>
#include <fmt/format.h>

typedef uint32_t Rgb;
struct Color {    
    uint8_t r;
    uint8_t g;
    uint8_t b;    
    uint8_t a;

    Color(uint8_t r, uint8_t g, uint8_t b) {
        this->a = 255; this->r = r; this->g = g; this->b = b;
    }

    Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
        this->a = a; this->r = r; this->g = g; this->b = b;
    }

    Color(Rgb rgb) 
        : Color((rgb & 0xff000000) >> 24, 
                (rgb & 0x00ff0000) >> 16, 
                (rgb & 0x0000ff00) >> 8, 
                (rgb & 0x000000ff)) {}

    Color(const std::string& hexstr) : Color(std::stoi(hexstr.substr(1), nullptr, 16)) {
        if(hexstr.length() < 8) {
            a = 255;
        }
    }

    std::string toString() const {
        return fmt::format("#{:06X}", this->rgb_noa());
    }

    void alpha_blend(Color c_new) {
            r = (c_new.a * c_new.r + ((255 - c_new.a) * r)) / 255;
            g = (c_new.a * c_new.g + ((255 - c_new.a) * g)) / 255;
            b = (c_new.a * c_new.b + ((255 - c_new.a) * b)) / 255;
            a = (c_new.a * c_new.a + ((255 - c_new.a) * a)) / 255;
    }

    bool operator==(const Color& other) const {
        return rgba() == other.rgba();
    }

    Color() : Color(0,0,0,0) {}

    inline int red() const {
        return r;
    }
    inline int green() const {
        return g;
    }
    inline int blue() const {
        return b;
    }
    inline int alpha() const {
        return a;
    }

    inline Rgb rgb() const {
        return 0xff000000 | (r << 16) | (g << 8) | b;
    }

    inline Rgb rgb_noa() const {
        return (r << 16) | (g << 8) | b;
    }


    inline Rgb rgba() const {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    inline double redF() const {
        return r / 255.0;
    }
    inline double greenF() const {
        return g / 255.0;
    }

    inline double blueF() const {
        return b / 255.0;
    }
} __attribute__((packed));
#endif // COLOR_H
