#include "xmllyrics.h"
#include <iostream>

int main(int argc, char* argv[]) {

    XMLLyrics xl("/home/christoph/vcs/mkcdg/test.xml");

    for(const auto& block: xl.blocks) {
        for(const auto& track: block.tracks) {
            for(const auto& line: track.lines) {
                for(const auto& word: line.words) {
                    for(const auto& syl: word.syllables){
                        std::cout << syl.text;
                    }
                    std::cout << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

        std::cout << "---" << std::endl;
    }
}
