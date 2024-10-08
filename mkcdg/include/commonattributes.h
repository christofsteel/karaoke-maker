//
// Created by christoph on 25.03.19.
//

#ifndef MKCDG_COMMONATTRIBUTES_H
#define MKCDG_COMMONATTRIBUTES_H

#include <string>
#include "color.h"
#include "tinyxml2.h"

enum BGType {
    BG_IMAGE,
    BG_COLOR,
};

enum VAlign {
    VTop,
    VMiddle,
    VBottom
};

enum HAlign {
    HLeft,
    HCenter,
    HRight
};

struct CommonAttributes {
public:
    Color unsung_color;
    Color sung_color;
    std::string fontname;
    uint fontsize;
    BGType bgtype;
    std::string background;
    VAlign valign;
    HAlign halign;

    size_t starttime;
    size_t endtime;
    std::string text;
    uint glyphCount;

    void load(tinyxml2::XMLElement *e) {
        setColorAttribute(unsung_color, "#00FFFF", "unsung", e);
        setColorAttribute(sung_color, "#FF00FF", "sung", e);
        setStringAttribute(fontname, "Dejavu Sans", "fontname", e);
        setUintAttribute(fontsize, "12", "fontsize", e);
        setBGtypeAttribute(bgtype, "color", "bgtype", e);
        setStringAttribute(background, "#000000", "bg", e);
        setVAlignAttribute(valign, "middle", "valign", e);
        setHAlignAttribute(halign, "center", "halign", e);
    }

    CommonAttributes() = default;

    CommonAttributes(tinyxml2::XMLElement *e) {
        load(e);
    }

    CommonAttributes(tinyxml2::XMLElement *e,const CommonAttributes& parent) {
        setColorAttribute(unsung_color, parent.unsung_color.toString(), "unsung", e);
        setColorAttribute(sung_color, parent.sung_color.toString(), "sung", e);
        setStringAttribute(fontname, parent.fontname, "fontname", e);
        setUintAttribute(fontsize, std::to_string(parent.fontsize), "fontsize", e);
        setBGtypeAttribute(bgtype, bgTypeToString(parent.bgtype), "bgtype", e);
        setStringAttribute(background, parent.background, "bg", e);
        setVAlignAttribute(valign, valignToString(parent.valign), "valign", e);
        setHAlignAttribute(halign, halignToString(parent.halign), "halign", e);
    }

    static std::string bgTypeToString(const BGType bg) {
        switch(bg) {
            case BG_COLOR:
                return "color";
            case BG_IMAGE:
                return "image";
        }
        return "color";
    }

    static std::string halignToString(const HAlign ha) {
        switch (ha) {
            case HLeft:
                return "left";
            case HRight:
                return "right";
            case HCenter:
                return "center";
        }
        return "center";
    }

    static std::string valignToString(const VAlign va) {
        switch (va) {
            case VTop:
                return "top";
            case VBottom:
                return "bottom";
            case VMiddle:
                return "middle";
        }
        return "middle";
    }

    static std::string getAttributeWithDefault(const std::string& defaultstring, const std::string& attributename, const tinyxml2::XMLElement *e) {
        std::string attribute;
        if(e->Attribute(attributename.c_str())) {
            attribute = e->Attribute(attributename.c_str());
            if(attribute.size() == 0) {
                attribute = defaultstring;
            }
        } else {
            attribute = defaultstring;
        }
        return attribute;
    }

    static void setStringAttribute(std::string& attribute, const std::string& defaultstring, const std::string& attributename, const tinyxml2::XMLElement *e) {
        attribute = getAttributeWithDefault(defaultstring, attributename, e);
    }

    static void setColorAttribute(Color& attribute, const std::string& defaultcolor, const std::string& attributename, const tinyxml2::XMLElement *e) {
        std::string str_attribute = getAttributeWithDefault(defaultcolor, attributename, e);
        attribute = Color(str_attribute);
        int a = 1;
    }

    static void setUintAttribute(uint& attribute, const std::string& defaultuint, const std::string& attributename, const tinyxml2::XMLElement *e) {
        std::string str_attribute = getAttributeWithDefault(defaultuint, attributename, e);
        attribute = std::stoi(str_attribute);
    }

    static void setBGtypeAttribute(BGType& attribute, const std::string& defaulttype, const std::string& attributename, const tinyxml2::XMLElement *e) {
        std::string str_attribute = getAttributeWithDefault(defaulttype, attributename, e);
        if(str_attribute == "color") {
            attribute = BG_COLOR;
        } else if (str_attribute == "image") {
            attribute = BG_IMAGE;
        } else {
            attribute = BG_COLOR;
        }
    }

    static void setHAlignAttribute(HAlign& attribute, const std::string& defaulttype, const std::string& attributename, const tinyxml2::XMLElement *e) {
        std::string str_attribute = getAttributeWithDefault(defaulttype, attributename, e);
        if(str_attribute == "left") {
            attribute = HLeft;
        } else if (str_attribute == "center") {
            attribute = HCenter;
        } else if (str_attribute == "right") {
            attribute = HRight;
        } else {
            attribute = HCenter;
        }
    }

    static void setVAlignAttribute(VAlign& attribute, const std::string& defaulttype, const std::string& attributename, const tinyxml2::XMLElement *e) {
        std::string str_attribute = getAttributeWithDefault(defaulttype, attributename, e);
        if(str_attribute == "top") {
            attribute = VTop;
        } else if (str_attribute == "middle") {
            attribute = VMiddle;
        } else if (str_attribute == "bottom") {
            attribute = VBottom;
        } else {
            attribute = VMiddle;
        }
    }

};


#endif //MKCDG_COMMONATTRIBUTES_H
