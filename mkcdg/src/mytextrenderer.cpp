#include "mytextrenderer.h"

void Mytextrenderer::renderTitle(Image &image) {
    int x = (image.width() - m_lyrics.titleImage.width()) / 2;
    int y = (image.height() - m_lyrics.titleImage.height()) / 2;
    drawBackground(m_lyrics.ca, image);
    image.drawImage(m_lyrics.titleImage, x, y);
}

Status Mytextrenderer::update(size_t timing, Image &image) {
    if (timing <= m_lyrics.titleTiming) {
        renderTitle(image);
        return OLD_BLOCK;
    } else {
        Block *block = m_lyrics.blockByTiming(timing);
        std::vector<Image> trackImages;
        if (block != nullptr) {
            if(!block->needsUpdating(timing)) {
                return NO_CHANGE;
            }
            drawBackground(block->ca, image);
            int overallHeight = 0;
            for (auto &track : block->tracks) {
                trackImages.push_back(track.renderTrack(timing));
                overallHeight += trackImages.back().height();
            }
            ssize_t padding = (image.height() - overallHeight) / block->tracks.size();

            size_t y = padding / 2;
            for (auto const &track : block->tracks) {
                size_t tracknr = &track - &block->tracks[0];
                size_t x = 0;
                switch (track.ca.halign) {
                    case HLeft: {
                        x = 0;
                        break;
                    }
                    case HRight: {
                        x = image.width() - trackImages[tracknr].width();
                        break;
                    }
                    case HCenter: {
                        x = (image.width() - trackImages[tracknr].width()) / 2;
                    }
                }
                image.drawImage(trackImages[tracknr], x, y);
                y += trackImages[tracknr].height();
                y += padding;
            }
        }
        if (block == oldblock || block == nullptr) {
            return OLD_BLOCK;
        } else {
            oldblock = block;
            return NEW_BLOCK;
        }
    }
}

bool Mytextrenderer::drawBackground(const CommonAttributes &ca, Image& image) {
    if(ca.bgtype == bgCacheType && ca.background == bgCache) {
        image.drawImage(bgCacheImage);//, 0,0, image.width(), image.height());
        return false;
    }
    switch(ca.bgtype) {
        case BG_COLOR: {
            Color c(ca.background);
            image.fill(c);
            bgCacheImage.fill(c);
            bgCache = ca.background;
            bgCacheType = ca.bgtype;
            return true;
        }
        case BG_IMAGE: {
            Image loader (ca.background);
            Image scaled(loader.scaled(image.width(), image.height()));
            image.drawImage(scaled);
            bgCacheImage.drawImage(scaled);//, 0, 0, image.width(), image.height());
            bgCache = ca.background;
            bgCacheType = ca.bgtype;
            return true;
        }
    }

}
