/**************************************************************************
 *  Karlyriceditor - a lyrics editor and CD+G / video export for Karaoke  *
 *  songs.                                                                *
 *  Copyright (C) 2009-2013 George Yunaev, support@ulduzsoft.com          *
 *                                                                        *
 *  This program is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *																	      *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#include "lyrics.h"
#include "xmllyrics.h"
#include "cdggenerator.h"
#include "color.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "image.h"
#include "font.h"
#include "args.hxx"


int main(int argc, char *argv[])
{
    args::ArgumentParser parser("Creates CDG files");
    args::HelpFlag arg_help(parser, "help", "prints help", {'h', "help"});
    args::ValueFlag<std::string> arg_font(parser, "fontname", "Name of a font in fontcache syntax", {'f', "font"}, "Dejavu Sans:Bold");
    args::ValueFlag<uint32_t> arg_fontsize(parser, "pixel", "Fontsize", {'p', "font-size"}, 16);
    args::ValueFlag<size_t> arg_duration(parser, "seconds", "Duration of the song", {'d', "duration"}, 0);
    args::ValueFlag<std::string> arg_artist(parser, "artist", "Artist name", {'a', "artist"}, "Unknown Artist");
    args::ValueFlag<std::string> arg_by(parser, "info", "Additional info on titlepage", {'t', "subcaption"}, "by mkCDG");
    args::ValueFlag<std::string> arg_title(parser, "title", "Title of the song", {'t', "title"}, "Unknown Title");
    args::ValueFlag<std::string> arg_info(parser, "color", "Info color in RRGGBB Hex ", {'i', "info"}, "ffffff");
    args::ValueFlag<std::string> arg_unsung(parser, "color", "Unsung color in RRGGBB Hex ", {'u', "unsung"}, "ffffff");
    args::ValueFlag<std::string> arg_sung(parser, "color", "Song color in RRGGBB Hex ", {'s', "sung"}, "0000ff");
    args::ValueFlag<std::string> arg_bg(parser, "color", "Background color in RRGGBB Hex ", {'b', "background"}, "000000");
    args::Flag arg_aa(parser, "anti alias", "Enable font antialiasing", {'x', "antialias"}, false);
    args::Positional<std::string> arg_inputfile(parser, "infile", "Input file");
    args::Positional<std::string> arg_outputfile(parser, "outfile", "Output file");

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        parser.Help(std::cerr);
        return 1;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        parser.Help(std::cerr);
        return 1;
    }

    if(!arg_inputfile || !arg_outputfile) {
        std::cerr << "No input or outputfile" << std::endl;
        return 1;
    }


    /*std::ifstream input(args::get(arg_inputfile));
    std::vector<std::string> lyrics_lines;

    std::string line;
    while (std::getline(input, line)) {
        lyrics_lines.push_back(line);
    }
    input.close();

    Lyrics lyrics;
    lyrics.load(lyrics_lines);*/

    XMLLyrics lyrics = XMLLyrics(args::get(arg_inputfile));
    CDGGenerator gen(args::get(arg_outputfile), lyrics);
    gen.myGenerate();



    /*CDGGenerator gen(
                args::get(arg_outputfile),
            Color(args::get(arg_bg)),
                Color(args::get(arg_info)),
                Color(args::get(arg_sung)),
                Color(args::get(arg_unsung)),
                args::get(arg_aa),
                Font::getFontFile(args::get(arg_font)),
                args::get(arg_fontsize),
                args::get(arg_artist),
                args::get(arg_title)
                );

    gen.generate( lyrics, args::get(arg_duration) );*/

    return 0;
    }
