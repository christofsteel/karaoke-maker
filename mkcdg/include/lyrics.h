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

#ifndef LYRICS_H
#define LYRICS_H

#include <vector>
#include <string>
#include <regex>

#include "lyricsevents.h"


class Lyrics
{
	public:
		Lyrics();
		~Lyrics();

        bool load(const std::vector<std::string>& lyric_lines);

		static const int PITCH_NOTE_FREESTYLE = (1 << 17);
		static const int PITCH_NOTE_GOLDEN = (1 << 18);

		//
		// Those functions are used to access lyrics
		//
		typedef struct
		{
            int64_t	timing;
            std::string	text;		// May be empty
			int		pitch;		// -1 if not set, used for Ultrastar
		} Syllable;

        typedef std::vector<Syllable>	Line;
        typedef std::vector<Line>			Block;

		// Lyrics are composed from one or more blocks, depending on mode
		// A block can contain one or more lines (up to the whole text)
		// A line can contain one or more syllables
		bool	isEmpty() const;
		int		totalBlocks() const;

		//
		// Those functions are used during lyric scanning
		//
        int64_t timeToMark( std::string data )
        {
            std::regex rxtime( "^(\\d+):(\\d+)\\.(\\d+)$");

            std::smatch match;
            if ( !std::regex_match(data, match, rxtime ) )
                return -1;

            int32_t min = std::stoi(match[1]);
            int32_t sec = std::stoi(match[2]);
            int32_t msec = std::stoi(match[3]);

            return min * 60000 + sec * 1000 + msec * 10;
        }

		// Indicates the lyrics are being built
		void	beginLyrics();

		// Set the time for current lyric
        void	curLyricSetTime( int64_t timems );

		// Add background event
        bool	addBackgroundEvent( int64_t timing, const std::string& text );

		// Set the pitch for current lyric
		void	curLyricSetPitch( int pitch );

		// Append text to current lyric
        void	curLyricAppendText( const std::string& text );

		// Adds the currently set lyric
		void	curLyricAdd();

		// Add "end of line". Multiple end of lines mean end of paragraph. This implies curLyricAdd()
		void	curLyricAddEndOfLine();

		// Indicates the lyrics are being built. No curlyric* functions may be called.
		void	endLyrics();

		// Clear the lyrics
		void	clear();

		// Pitch text representation
        //static std::string pitchToNote( int pitch, bool show_octave = true );

		// Returns the events
        LyricsEvents* events();
        std::vector<Block>	m_lyrics;

	private:
        LyricsEvents	m_events;

		// Used during scanning lyrics
		bool			m_scanning;
		int				m_added_eofs;
		Syllable		m_currentLyric;
};


#endif // LYRICS_H
