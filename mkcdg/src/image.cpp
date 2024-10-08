#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


Image::Image(size_t width, size_t height) {
    data.resize(width * height);
   m_height = height;
   m_width = width;
}

Image::Image(const std::string &filename) {
    int width, height;
    uint8_t* image_data = stbi_load(filename.c_str(), &width, &height, nullptr, 4);
    if(image_data) {

    m_width = static_cast<size_t>(width);
    m_height= static_cast<size_t>(height);

    data.assign(reinterpret_cast<Color*>(image_data), reinterpret_cast<Color*>(image_data) + (m_width * m_height));
    } else {
        std::cout << "Warning: Could not load " << filename << std::endl;
        m_width = 100;
        m_height = 100;
        data.resize(100 * 100);
        fill(0x00000000);
}}

void Image::save(const std::string& filename) const {
    stbi_write_png(filename.c_str(), m_width, m_height, 4, &data[0], 0);
}

bool Image::load(const std::string &filename) {
    int width, height;
    uint8_t* image_data = stbi_load(filename.c_str(), &width, &height, nullptr, 4);
    if(image_data) {

    m_width = static_cast<size_t>(width);
    m_height= static_cast<size_t>(height);

    data.assign(reinterpret_cast<Color*>(image_data), reinterpret_cast<Color*>(image_data) + (m_width * m_height));
    return true;
    } else {
        std::cout << "Warning: Could not load " << filename << std::endl;
        m_width = 100;
        m_height = 100;
        data.resize(100 * 100);
        fill(0x00000000);
        return false;
}}

size_t Image::width() const
{
    return m_width;
}

size_t Image::height() const
{
    return m_height;
}
const Color* Image::scanLine(size_t line) const
{
    return &data[line * m_width];
}

void Image::drawImage(const Image& source, /* size_t sx, size_t sy, size_t sw,size_t sh, */size_t x, size_t y)
{
    for (size_t j = 0; j < source.height(); j++) {
        for(size_t i = 0; i < source.width(); i++) {
            if ((y+j) * m_width + x+i < data.size()) {
            data[(y+j) * m_width + x+i].alpha_blend(source.data[j * source.m_width + i]);
            }
        }
    }
}

Image Image::scaled(double width, double height)
{
    size_t i_width = static_cast<size_t>(width);
    size_t i_height = static_cast<size_t>(height);

    double x_scale = this->m_width / width;
    double y_scale = this->m_height / height;


    Image img;
    img.m_width = i_width;
    img.m_height = i_height;
    img.data.resize(i_width * i_height);
    for (size_t i = 0; i < i_width; i++) {
        for (size_t j = 0; j < i_height; j++){
            size_t x = i * x_scale;
            size_t y = j * y_scale;
            img.data[j * i_width + i] = this->data[y * m_width +x ];
        }
    }

    return img;
}

/*
void Image::drawRect(size_t x, size_t y, size_t w, size_t h)
{
    for (size_t j = x; j < h; j++) {
        for(size_t i = y; i < w; i++) {
            if (j * m_width + i < data.size()) {
            data[j * m_width + i] = brush;
            }
        }
    }
}*/

Color Image::alpha_blend(Color c_old, Color c_new, uint8_t I) const {
    return Color(
            (I * c_new.r + ((255 - I) * c_old.r)) / 255,
            (I * c_new.g + ((255 - I) * c_old.g)) / 255,
            (I * c_new.b + ((255 - I) * c_old.b)) / 255
            );
}
