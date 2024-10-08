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

#ifndef CDGGENERATOR_H
#define CDGGENERATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "cdg.h"
#include "color.h"
#include "image.h"
#include "xmllyrics.h"
#include "mytextrenderer.h"

class CDGGenerator
{
	public:
        /*CDGGenerator( const std::string& outputFile
                    , const Color& colorBackground
                    , const Color& colorInfo
                    , const Color& colorInactive
                    , const Color& colorActive
                    , bool antiAlias = true
                    , const std::string& fontName = "Droid Sans"
                    , uint32_t fontSize = 12
                    , const std::string& artist = "Unknown"
                    , const std::string& title = "Unknown"
                    , const std::string& createdBy = "mkCDG"
                    , size_t titleTime = 5000
                    );*/

        CDGGenerator( const std::string& outputFile, XMLLyrics& lyrics);

		// Generate the CD+G lyrics
        //void	generate( XMLLyrics& lyrics, size_t total_length );
        void myGenerate();

		// Returns the CD+G stream
        //QByteArray	stream();
        std::vector<char> stream();
        void	applyTileChanges( const Image& orig, Image* newimg );

	private:
        void	addColorGradations( const Color& color, unsigned int number );
		void	addSubcode( const SubCode& sc );
		void	addEmpty();
        void	addLoadColors( const Color& bgcolor, const Color& titlecolor,
                               const Color& actcolor, const Color& inactcolor );
		void	clearScreen();

        void	fillColor( char * buffer, const Color& color );
        int		getColor( Rgb color );
        void    addColor( const Color& color );
        void	checkTile( int offset_x, int offset_y, const Image* orig, Image* newimg );

    private:
        std::string                 m_outputFile;
        Color					m_colorBackground;
        Color					m_colorInfo;
        Color					m_colorActive;
        Color					m_colorInactive;
        Font                   m_renderFont;
        std::string                 m_artist;
        std::string                 m_title;
        std::string                 m_createdBy;
        size_t                  m_titleTime;

        std::vector< SubCode >		m_stream;			// CD+G stream
        std::vector< Color >		m_colors;			// 16 colors used in CD+G
        int						m_streamColorIndex; // Reserved space for colors
        //Project		*			m_project;
        XMLLyrics& m_lyrics;

        bool                    m_enableAntiAlias;
};


#endif // CDGGENERATOR_H
