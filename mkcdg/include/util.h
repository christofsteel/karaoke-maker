#ifndef UTIL_H
#define UTIL_H

#include <string>

struct Size {
    size_t x,y,w,h;
    Size(size_t x, size_t y, size_t w, size_t h) {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    Size(size_t w, size_t h){
        this->x = 0;
        this->y = 0;
        this->w = w;
        this->h = h;
    }
};

static inline std::string trim (const std::string& string) {
    size_t first = string.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return string;
    }
    size_t last = string.find_last_not_of(' ');
    return string.substr(first, (last - first + 1));
}

#endif // UTIL_H
