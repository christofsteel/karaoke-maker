//
// Created by christoph on 25.03.19.
//

#include "track.h"

Track::Track(tinyxml2::XMLElement* e_track, const CommonAttributes& pca,
             const std::map<std::string, std::map<std::string, std::string>> tca) {
    if(e_track->Attribute("ref") == nullptr) {
        throw std::runtime_error("Missing attribute ref for track");
    }
    ref = e_track->Attribute("ref");
    const auto attr_map = tca.at(ref);

    std::string unsung = findInRef("unsung", attr_map, pca.unsung_color.toString());
    ca.setColorAttribute(ca.unsung_color, unsung, "unsung", e_track);

    std::string sung = findInRef("sung", attr_map, pca.sung_color.toString());
    ca.setColorAttribute(ca.sung_color, sung, "sung", e_track);

    std::string fontname = findInRef("font", attr_map, pca.fontname);
    ca.setStringAttribute(ca.fontname, fontname, "font", e_track);

    std::string fontsize = findInRef("fontsize", attr_map, std::to_string(pca.fontsize));
    ca.setUintAttribute(ca.fontsize, fontsize, "fontsize", e_track);

    std::string bgtype = findInRef("bgtype", attr_map, CommonAttributes::bgTypeToString(pca.bgtype));
    ca.setBGtypeAttribute(ca.bgtype, bgtype, "bgtype", e_track);

    std::string bg = findInRef("bg", attr_map, pca.background);
    ca.setStringAttribute(ca.background, bg, "bg", e_track);

    std::string valign = findInRef("valign", attr_map, CommonAttributes::valignToString(pca.valign));
    ca.setVAlignAttribute(ca.valign, valign, "valign", e_track);

    std::string halign = findInRef("halign", attr_map, CommonAttributes::halignToString(pca.halign));
    ca.setHAlignAttribute(ca.halign, halign, "halign", e_track);

    size_t last_endtime = 0;
    height = 0;
    for(auto e_line = e_track->FirstChildElement("line"); e_line != nullptr; e_line = e_line->NextSiblingElement("line")) {
        Line line(e_line, ca);
        lines.push_back(line);
        if (line.ca.starttime < last_endtime) {
            throw std::runtime_error("Start before end");
        }
        last_endtime = line.ca.endtime;
        ca.text.append(line.ca.text);
        ca.text.append("\n");
        height += line.sungLine.height();
    }
    if(ca.text.length() == 0) {
        throw std::runtime_error("Empty Track");
    }
    ca.text.pop_back();
    ca.starttime = lines.front().ca.starttime;
    ca.endtime = lines.back().ca.endtime;

}

Image Track::renderTrack(size_t timing) {
    size_t height = 0;
    size_t width = 0;

    for(auto const& line: lines) {
        height += line.unsungLine.height();
        width = std::max(width, line.unsungLine.width());
    }

    Image trackImg(width, height);
    trackImg.fill(0x00000000);

    size_t y = 0;
    for(auto& line: lines) {
        size_t x=0;
        Image lineImage(line.renderLine(timing));
        switch(line.ca.halign) {
            case HRight: {
                x=width - lineImage.width();
                break;
            }
            case HCenter: {
                x = (width - lineImage.width()) / 2;
                break;
            }
            case HLeft: {
                x = 0;
                break;
            }
        }
        trackImg.drawImage(lineImage, x, y);
        y += line.unsungLine.height();
    }
    return trackImg;
}
