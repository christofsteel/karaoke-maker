#include "image.h"
#include "xmllyrics.h"

int main(int argc, char* argv[]) {
    Image image(640,480);
    image.fill(Color(0xff000000));
    image.setPen(Color(0xff00ff00));

    XMLLyrics lyrics("/home/christoph/vcs/mkcdg/test.xml");
    Font font(Font::getFontFile(lyrics.ca.fontname), lyrics.ca.fontsize, false);

    image.drawText(40,40,"T est123", font, 2, Color(0xffff0000));


    image.save("/tmp/title.png");
    std::cout << lyrics.artist << std::endl;
    std::cout << lyrics.title << std::endl;
}
