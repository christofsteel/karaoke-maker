#ifndef MYTEXTRENDERER_H
#define MYTEXTRENDERER_H

#include "xmllyrics.h"
#include "image.h"

enum Status {
    NEW_BLOCK,
    OLD_BLOCK,
    NO_CHANGE
};

class Mytextrenderer
{
    public:
        Mytextrenderer(size_t width, size_t height, XMLLyrics& lyrics) : m_lyrics(lyrics), bgCacheImage(width, height) {
            bgCacheImage.fill(0xff000000);
            bgCacheType = BG_COLOR;
            bgCache = "#000000";
        }
        Status update(size_t timing, Image& image);
        void renderTitle(Image& image);
    private:
        bool drawBackground(const CommonAttributes& ca, Image& image);
        XMLLyrics& m_lyrics;
        Image bgCacheImage;
        BGType bgCacheType;
        const Block* oldblock;
        std::string bgCache;
};

#endif // MYTEXTRENDERER_H
