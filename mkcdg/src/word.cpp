//
// Created by christoph on 25.03.19.
//

#include "word.h"

Word::Word(tinyxml2::XMLElement* e_word, const CommonAttributes& pca) : ca(e_word, pca) {
    size_t last_endtiming = 0;
    for(auto e_syl = e_word->FirstChildElement("syllable"); e_syl != nullptr; e_syl = e_syl->NextSiblingElement("syllable")) {
        Syllable syl(e_syl, ca);
        syllables.push_back(syl);
        if(syl.ca.starttime < last_endtiming) {
            throw std::runtime_error("Start before end");
        }
        last_endtiming = syl.ca.endtime;
        ca.text.append(syl.ca.text);
    }
    if(ca.text.length() == 0) {
        throw std::runtime_error("Empty Word");
    }
    ca.starttime = syllables.front().ca.starttime;
    ca.endtime = syllables.back().ca.endtime;
}

