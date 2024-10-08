//
// Created by christoph on 25.03.19.
//

#ifndef MKCDG_WORD_H
#define MKCDG_WORD_H


#include <vector>
#include "tinyxml2.h"
#include "commonattributes.h"
#include "syllable.h"

class Word {
public:
    Word(tinyxml2::XMLElement* e_word, const CommonAttributes& pca);

    std::vector<Syllable> syllables;
    CommonAttributes ca;
};



#endif //MKCDG_WORD_H
