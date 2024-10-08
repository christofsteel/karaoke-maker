//
// Created by christoph on 25.03.19.
//

#ifndef MKCDG_LINE_H
#define MKCDG_LINE_H

#include "commonattributes.h"
#include "word.h"
#include "image.h"

class Line {
public:
    Line(tinyxml2::XMLElement* e_line, const CommonAttributes& pca);
    std::vector<Word> words;
    CommonAttributes ca;
    Image renderLine(size_t timing);
    Image sungLine;
    Image unsungLine;
    inline bool needsUpdating(size_t timing) {
        if(timing > ca.endtime
        || timing < ca. starttime
        || lastpos == timing_to_glyph(timing)) {
            return false;
        }
        lastpos = timing_to_glyph(timing);
        return true;
    }

private:
    std::vector<std::pair<Color, Color>> colorVec;
    std::vector<Font::GlyphBitmap> lineChars;
    int lastpos;
    std::vector<size_t> timing_by_glyph;

    inline size_t timing_to_glyph(size_t timing) const {
        for(int i=0; i < timing_by_glyph.size(); i++) {
            if(timing < timing_by_glyph[i]) {
                return i;
            }
        }
        return timing_by_glyph.size() + 1;
    }
};



#endif //MKCDG_LINE_H
