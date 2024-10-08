//
// Created by christoph on 25.03.19.
//

#include "line.h"

Line::Line(tinyxml2::XMLElement* e_line, const CommonAttributes& pca) : ca(e_line, pca) {
    size_t last_endtiming = 0;
    for(auto e_word = e_line->FirstChildElement("word"); e_word != nullptr; e_word = e_word->NextSiblingElement("word")) {
        Word word(e_word, ca);
        words.push_back(word);
        if(word.ca.starttime < last_endtiming) {
            throw std::runtime_error("Start before end");
        }
        last_endtiming = word.ca.endtime;
        ca.text.append(word.ca.text);
        ca.text.append(" ");
    }
    if(ca.text.length() == 0) {
        throw std::runtime_error("Empty Line");
    }
    ca.text.pop_back();
    ca.starttime = words.front().ca.starttime;
    ca.endtime = words.back().ca.endtime;


    Syllable curSyl = words.front().syllables.front();
    std::pair<std::string, uint> fontSpec(curSyl.ca.fontname, curSyl.ca.fontsize);

    Font font(Font::getFontFile(curSyl.ca.fontname), curSyl.ca.fontsize, false);

    //std::vector<Syllable*> sylGroup;
    std::string textToShape;
    size_t px_width = 0;
    size_t px_height = 0;
    int min_offset = 0;
    int last_y_advance = 0;

    for(auto& word: words) {
        for(auto& syl: word.syllables) {
            for(int i = 0; i < syl.ca.glyphCount; i++) {
                colorVec.push_back(std::pair<Color, Color>(syl.ca.unsung_color, syl.ca.sung_color));
                timing_by_glyph.push_back(syl.ca.starttime + i * syl.timePerGlyph);
            }
            if(syl.ca.fontname == fontSpec.first && syl.ca.fontsize == fontSpec.second) {
                textToShape.append(syl.ca.text);
            } else {
                for(auto const& glyph : font.renderText(textToShape)) {
                    lineChars.push_back(glyph);
                    px_width += glyph.x_advance;
                    px_height = std::max(px_height, glyph.height + glyph.y_offset + last_y_advance);
                    min_offset = std::min(min_offset, glyph.y_offset + last_y_advance);
                    last_y_advance = glyph.y_advance;
                }
                fontSpec.first = syl.ca.fontname;
                fontSpec.second = syl.ca.fontsize;
                font = Font(Font::getFontFile(syl.ca.fontname), syl.ca.fontsize, false);
                textToShape = syl.ca.text;
            }
        }
        textToShape.append(" ");
        colorVec.push_back(std::pair<Color, Color>(Color(0xff000000), Color(0xff000000)));
        timing_by_glyph.push_back(timing_by_glyph.back());
    }

    textToShape.pop_back();
    colorVec.pop_back();
    timing_by_glyph.pop_back();

    for(auto const& glyph : font.renderText(textToShape)) {
        lineChars.push_back(glyph);
        px_width += glyph.x_advance;
        px_height = std::max(px_height, glyph.height + glyph.y_offset + last_y_advance);
        min_offset = std::min(min_offset, glyph.y_offset + last_y_advance);
        last_y_advance = glyph.y_advance;
    }

    int baseline = std::abs(min_offset);
    px_height += baseline;
    sungLine.resize(px_width, px_height);
    unsungLine.resize(px_width, px_height);
    sungLine.fill(0x00000000);
    unsungLine.fill(0x00000000);

    uint x=0;
    uint y=0;
    for(auto& glyph : lineChars) {
        int pos = &glyph - &lineChars[0];
        Color unsungcolor(colorVec[pos].first.rgb());
        Color sungcolor(colorVec[pos].second.rgb());
        glyph.y_offset = px_height - (baseline + glyph.y_offset) - glyph.height;
        for (size_t j = 0; j < glyph.height; j++) {
            for (size_t i = 0; i < glyph.width; i++) {
                size_t buffercord = (i+x+glyph.x_offset) + (j+y+glyph.y_offset) * px_width;
                size_t fontcord = i + j * glyph.width;
                unsungcolor.a = glyph.data[fontcord];
                sungcolor.a = glyph.data[fontcord];
                if(buffercord < sungLine.data.size()) {
                    sungLine.data[buffercord].alpha_blend(sungcolor);
                    unsungLine.data[buffercord].alpha_blend(unsungcolor);
                }
            }
        }
        x += glyph.x_advance;
        y += glyph.y_advance;
    }
    lastpos = -1;
}

Image Line::renderLine(size_t timing) {
    if(timing < ca.starttime)
        return unsungLine;
    if(timing > ca.endtime)
        return sungLine;

    Image line(sungLine.width(), sungLine.height());
    line.fill(0x00000000);

    uint x=0;
    uint y=0;
    for(auto& glyph : lineChars) {
        int pos = &glyph - &lineChars[0];
        Color unsungcolor(colorVec[pos].first.rgb());
        Color sungcolor(colorVec[pos].second.rgb());

        for (size_t j = 0; j < glyph.height; j++) {
            for (size_t i = 0; i < glyph.width; i++) {
                size_t buffercord = (i+x+glyph.x_offset) + (j+y+glyph.y_offset) * line.width();
                size_t fontcord = i + j * glyph.width;
                unsungcolor.a = glyph.data[fontcord];
                sungcolor.a = glyph.data[fontcord];
                if(buffercord < sungLine.data.size()) {
                    if(timing < timing_by_glyph[pos]) {
                        line.data[buffercord].alpha_blend(unsungcolor);
                    } else {
                        line.data[buffercord].alpha_blend(sungcolor);
                    }
                }
            }
        }
        x += glyph.x_advance;
        y += glyph.y_advance;
    }
    return line;
}
