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
#include "util.h"

Lyrics::Lyrics()
{
	m_scanning = false;
	m_added_eofs = 0;

	m_currentLyric.timing = -1;
	m_currentLyric.pitch = -1;
}

Lyrics::~Lyrics()
{
}

bool Lyrics::load(const std::vector<std::string>& lyric_lines)
{
    /*if ( !validate() )
        return false;*/

    this->beginLyrics();

    for(const std::string& line: lyric_lines)
    {
        if ( trim(line).empty())
        {
            // end of paragraph
            this->curLyricAddEndOfLine();
            continue;
        }

        // Skip comments
        if ( trim(line)[0] == '#' )
            continue;

        std::string lyrictext, timing, special;
        bool in_time_tag = false;
        bool in_special_tag = false;
        int added_lyrics = 0;

        for ( size_t col = 0; col < line.size(); col++ )
        {
            if ( in_special_tag )
            {
                // Special tag ends?
                if ( line[col] == '}' )
                    in_special_tag = false;
                else
                    special.push_back( line[col] );
            }
            else if ( in_time_tag )
            {
                // Time tag ends?
                if ( line[col] == ']' )
                    in_time_tag = false;
                else
                    timing.push_back( line[col] );
            }
            else if ( line[col] == '{' )
            {
                in_special_tag = true;
            }
            else if ( line[col] == '[' )
            {
                // If we already have the text, add the lyric
                if ( !timing.empty() )
                {
                    if ( !special.empty() )
                        this->addBackgroundEvent( timeToMark( timing ), special );

                    // The first lyric should not be empty
                    if ( added_lyrics > 0 || !lyrictext.empty() )
                    {
                        this->curLyricSetTime( timeToMark( timing ) );
                        this->curLyricAppendText( lyrictext );
                        this->curLyricAdd();
                        added_lyrics++;
                    }

                    lyrictext.clear();
                    special.clear();
                    timing.clear();
                }

                in_time_tag = true;
            }
            else
                lyrictext.push_back( line[col] );
        }

        // There may be lyrics at the end of line
        if ( !timing.empty() )
        {
            if ( !special.empty() )
                this->addBackgroundEvent( timeToMark( timing ), special );

            this->curLyricSetTime( timeToMark( timing ) );
            this->curLyricAppendText( lyrictext );
            this->curLyricAdd();
        }

        this->curLyricAddEndOfLine();
    }

    this->endLyrics();
    return true;
}

void Lyrics::beginLyrics()
{
	m_scanning = true;
	m_added_eofs = 0;

	m_currentLyric.timing = -1;
	m_currentLyric.pitch = -1;
	m_currentLyric.text.clear();
}

void Lyrics::curLyricSetTime( int64_t timems )
{
	if ( !m_scanning )
		abort();

	m_currentLyric.timing = timems;
}

void Lyrics::curLyricSetPitch( int pitch )
{
	if ( !m_scanning )
		abort();

	m_currentLyric.pitch = pitch;
}

void Lyrics::curLyricAppendText( const std::string& text )
{
	if ( !m_scanning )
		abort();

	m_currentLyric.text += text;
}

void Lyrics::curLyricAdd()
{
	if ( !m_scanning )
		abort();

	// If time is not set, do not add
	if ( m_currentLyric.timing == -1 )
	{
		// Nothing to add
		m_currentLyric.pitch = -1;
		m_currentLyric.text.clear();

		return;
	}

	// Create a new block and new line, and fill the first entry
	// if there are no blocks or m_added_eofs > 1
    if ( m_lyrics.empty() || m_added_eofs > 1 )
	{
		Block block;
		Line line;

		line.push_back( m_currentLyric );
		block.push_back( line );
		m_lyrics.push_back( block );
	}
	else
	{
		Block& lastBlock = m_lyrics.back();

		// Create a new line and fill the first entry if there
		// are no lines, or m_added_eofs > 0
        if ( lastBlock.empty() || m_added_eofs > 0 )
		{
			Line line;

			line.push_back( m_currentLyric );
			lastBlock.push_back( line );
		}
		else
		{
			Line& lastLine = lastBlock.back();
			lastLine.push_back( m_currentLyric );
		}
	}

#if 0
	qDebug("block %d, line %d, entry %d: crlfs %d: %d %s",
		   m_lyrics.size() - 1,
		   m_lyrics.back().size() - 1,
		   m_lyrics.back().back().size() - 1,
		   m_added_eofs,
		   (int) m_currentLyric.timing,
		   qPrintable( m_currentLyric.text ) );
#endif

	// Reset the fields
	m_added_eofs = 0;

	m_currentLyric.timing = -1;
	m_currentLyric.pitch = -1;
	m_currentLyric.text.clear();
}

void Lyrics::curLyricAddEndOfLine()
{
	if ( !m_scanning )
		abort();

	curLyricAdd();
	m_added_eofs++;
}

void Lyrics::endLyrics()
{
	curLyricAdd();
	m_scanning = false;
}

int Lyrics::totalBlocks() const
{
	return m_lyrics.size();
}

bool Lyrics::isEmpty() const
{
    return m_lyrics.empty();
}

void Lyrics::clear()
{
	m_lyrics.clear();
}

/*QString Lyrics::pitchToNote( int pitch, bool )
{
	return QString::number( pitch );
}*/

bool Lyrics::addBackgroundEvent( int64_t timing, const std::string& text)
{
    return m_events.addEvent( timing, text);
}

LyricsEvents* Lyrics::events()
{
    return &m_events;
}
/*
int Lyrics::isSpecialSequence( const QString& line, int offset )
{
	if ( line[offset] != '@' || offset + 1 >= line.length() )
		return 0;

	offset++;

	if ( line[offset] == '#' && offset + 6 < line.length() )
		return 7;

	if ( (line[offset] == '!' || line[offset] == 'S') && offset + 1 < line.length() )
		return 1;

	return 0;
}

QString Lyrics::stripSpecialSequences( const QString& line )
{
	QString out;

	for ( unsigned int i = 0; i < line.length(); i++ )
	{
		int speclen = isSpecialSequence( line, i );

		if ( speclen != 0 )
		{
			i += speclen;
			continue;
		}

		out.push_back( line[i]);
	}

	return out;
}
*/
