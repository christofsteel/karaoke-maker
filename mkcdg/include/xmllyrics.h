#ifndef XMLLYRICS_H
#define XMLLYRICS_H

#include <vector>
#include <string>
#include <map>
#include "tinyxml2.h"

#include "image.h"
#include "color.h"
#include "font.h"
#include "block.h"


class XMLLyrics
{
public:
    XMLLyrics(const std::string& filename);

    std::string artist;
    std::string title;
    Color titleColor;
    Color borderColor;
    size_t prefetch;
    size_t titleTiming;
    size_t append;

    Image titleImage;

    CommonAttributes ca;

    Block* blockByTiming(size_t timing);

    std::vector<Block> blocks;
    int currentBlock = 0;

private:
    Image renderTitle() const;
    std::map<std::string, std::map<std::string, std::string>> parseTrackSettings(tinyxml2::XMLElement *e);
    Image renderString(Font font, Color c, std::string str) const;

};

#endif // XMLLYRICS_H
