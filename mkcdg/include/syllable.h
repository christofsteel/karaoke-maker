//
// Created by christoph on 25.03.19.
//

#ifndef MKCDG_SYLLABLE_H
#define MKCDG_SYLLABLE_H


#include "tinyxml2.h"
#include "commonattributes.h"

class Syllable {
public:
    Syllable(tinyxml2::XMLElement* e_syl, const CommonAttributes& pca);
    CommonAttributes ca;

    double timePerGlyph;
};




#endif //MKCDG_SYLLABLE_H
