//
// Created by christoph on 25.03.19.
//

#ifndef MKCDG_BLOCK_H
#define MKCDG_BLOCK_H


#include "tinyxml2.h"
#include "commonattributes.h"
#include <map>
#include "track.h"

class Block {
public:
    Block(tinyxml2::XMLElement* e_block, const CommonAttributes& pca, const std::map<std::string, std::map<std::string, std::string>> tca, int last_timing)
            : ca(e_block, pca) {
        size_t endtiming = 0;
        size_t starttiming = last_timing;

        for(auto e_track = e_block->FirstChildElement("track"); e_track != nullptr; e_track = e_track->NextSiblingElement("track")) {
            Track track(e_track, ca, tca);
            tracks.push_back(track);
            endtiming = std::max(endtiming, track.ca.endtime);
            starttiming = std::min(starttiming, track.ca.starttime);
        }
        ca.starttime = starttiming;
        ca.endtime = endtiming;
    }

    inline bool needsUpdating(size_t timing) {
        if(!already_shown && timing >= ca.starttime) {
            already_shown = true;
            return true;
        }
        for(auto& track: tracks) {
            if(track.needsUpdating(timing)) {
                return true;
            }
        }
        return false;
    }

    CommonAttributes ca;
    std::vector<Track> tracks;
    bool already_shown = false;

};



#endif //MKCDG_BLOCK_H
