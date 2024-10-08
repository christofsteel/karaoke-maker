//
// Created by christoph on 25.03.19.
//

#include "font.h"
#include "syllable.h"

Syllable::Syllable(tinyxml2::XMLElement* e_syl, const CommonAttributes& pca) : ca(e_syl, pca) {
    if(e_syl->GetText() == nullptr) {
        throw std::runtime_error("Empty syllable detected");
    }
    ca.text = e_syl->GetText();
    if(ca.text.length() == 0) {
        throw std::runtime_error("Empty syllable detected");
    }

    const auto a_starttime = e_syl->Attribute("start");
    const auto a_endtime = e_syl->Attribute("end");

    if(a_starttime && a_endtime) {
        ca.starttime = std::stol(a_starttime);
        ca.endtime = std::stol(a_endtime);
    } else {
        throw std::runtime_error("Syllable has not start or end time");
    }

    if (ca.endtime < ca.starttime)
        throw std::runtime_error("Syllable has end before start time");


    Font font(Font::getFontFile(ca.fontname), ca.fontsize, false);
    ca.glyphCount = font.getGlyphCount(ca.text);
    timePerGlyph = static_cast<double>((ca.endtime - ca.starttime)) / ca.glyphCount;
}
