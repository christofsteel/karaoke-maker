//
// Created by christoph on 25.03.19.
//

#ifndef MKCDG_TRACK_H
#define MKCDG_TRACK_H


#include <string>
#include <map>
#include "line.h"
#include "tinyxml2.h"

class Track {
public:
    Track(tinyxml2::XMLElement* e_track, const CommonAttributes& pca,
          const std::map<std::string, std::map<std::string, std::string>> tca);
    size_t height;
    CommonAttributes ca;
    std::vector<Line> lines;
    Image renderTrack(size_t timing);

    inline bool needsUpdating(size_t timing) {
        bool update;
        for(auto & line:lines) {
            update = update || line.needsUpdating(timing);
        }
        return update;
    }

private:
    static std::string findInRef(const std::string& attr, const std::map<std::string, std::string>& map, const std::string& fallback) {
        try {
            return map.at(attr);
        } catch (std::out_of_range e) {
            return fallback;
        }
    }

    std::string ref;
};


#endif //MKCDG_TRACK_H
