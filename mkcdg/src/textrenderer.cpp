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

#include <sstream>
#include "textrenderer.h"
#include "util.h"
#include <cctype>
#include <fmt/format.h>


// Some preamble constants
static const int PREAMBLE_SQUARE = 500; // 500ms for each square
static const int PREAMBLE_MIN_PAUSE = 5000; // 5000ms minimum pause between verses for the preamble to appear

// Font size difference
static const int SMALL_FONT_DIFF = 4; // 4px less


TextRenderer::TextRenderer( int width, int height, const XMLLyrics& in_lyrics) : lyrics(in_lyrics)

{
    //m_currentAlignment = VerticalBottom;
    //m_image = new Image( width, height);//, QImage::Format_ARGB32 );
	init();
}


size_t TextRenderer::getLastTime(){
    return m_lyricBlocks.back().timeend;
}

/*void TextRenderer::setLyrics( XMLLyrics& lyrics )
{
	init();
*/
	// Store the empty block at position 0 which will serve as future title placeholder
    /*LyricBlockInfo titleblock;
	titleblock.timestart = 0;
	titleblock.timeend = 0;
	m_lyricBlocks.push_back( titleblock );

    for(auto const& block : lyrics.blocks) {
        bool intitle = false;
        bool hadtext = false;
        LyricBlockInfo binfo;
        binfo.timestart = block.front().front().timing;
        binfo.timeend = block.back().back().timing;

        for(auto const& line : block) {
            int64_t endlinetime = line.back().timing;

            if(!line.back().text.empty()) {
                int64_t calcenlinetime;

            }
        }
    }

	// Compile the lyrics
	for ( int bl = 0; bl < lyrics.totalBlocks(); bl++ )
	{
		const Lyrics::Block& block = lyrics.block( bl );

		if ( block.size() == 0 )
			continue;

		bool intitle = false;
		bool hadtext = false;
		LyricBlockInfo binfo;
        binfo.timestart = block.front().front().timing;
        binfo.timeend = block.back().back().timing;

		for ( int ln = 0; ln < block.size(); ln++ )
		{
			const Lyrics::Line& line = block[ln];

			// Calculate the time the line ends
            int64_t endlinetime = line.back().timing;

			// If the last timing tag for this line is empty, this is the end.
			// We prefer it, but if it is not the case, we'll find something else.
            if ( !line.back().text.empty() )
			{
                int64_t calcenlinetime;

				if ( ln + 1 < block.size() )	// Beginning of next line in this block
                    calcenlinetime = block[ln].front().timing;
				else if ( bl + 1 < lyrics.totalBlocks() ) // Beginning of next block
                    calcenlinetime = block[ln+1].back().timing;
				else // last line in last block
					calcenlinetime = endlinetime + 2000; // 2 sec

                endlinetime = std::min( calcenlinetime, endlinetime + 2000 );
			}

			// Last item must be empty, so it is ok
			for ( int pos = 0; pos < line.size(); pos++ )
			{
				Lyrics::Syllable lentry = line[pos];

                if ( trim(lentry.text).empty() && !hadtext )
					continue;

                if ( lentry.text.empty() )
					continue;

                int64_t starttime = line[pos].timing;
                int64_t endtime = (pos + 1 == line.size()) ? endlinetime : line[pos+1].timing;

				// Reset the block start time in case all previous lines were empty
				if ( !hadtext )
				{
					hadtext = true;
					binfo.timestart = starttime;
				}

                compileLine( lentry.text, starttime, endtime, &binfo, &intitle );
			}

			binfo.text += "\n";
			binfo.offsets [ endlinetime ] = binfo.text.size() - 1;
		}

        binfo.text = trim(binfo.text);

		// Do not add the blocks with no text
        if ( !binfo.text.empty() )
			m_lyricBlocks.push_back( binfo );
	}
    m_lyricEvents = lyrics.events();
    prepareEvents();*/
//}

/*void TextRenderer::compileLine( const std::string& line, int64_t starttime, int64_t endtime, LyricBlockInfo * binfo, bool *intitle )
{
	// Drawn text string
    std::string drawntext;

	// Stores the character offsets which change their color with time
    std::vector<int> timedcharacters;

	int blocktextstart = binfo->text.size();

	for ( int ch = 0; ch < line.length(); ch++ )
	{
        char lchar = line[ch];

		// Parse the special sequences
		if ( lchar == '@' )
		{
            // Title start/end: @$title
			if ( ch + 1 < line.length() && line[ch+1] == '$' )
			{
				*intitle = !*intitle;
				ch++;
				continue;
			}

            // Color change: @#00C0C0
			if ( ch + 7 < line.length() && line[ch+1] == '#' )
			{
				// Store the new 'unsung' color for this position
                std::string colorname = line.substr( ch + 2, 6 );
				binfo->colors[ blocktextstart + drawntext.length() ] = colorname;
				ch += 7;
				continue;
			}

            // Font size change down: @<
			if ( ch + 1 < line.length() && line[ch+1] == '<' )
			{
				binfo->fonts[ blocktextstart + drawntext.length() ] = -SMALL_FONT_DIFF; // make the font smaller
				ch += 1;
				continue;
			}

            // Font size change up: @>
			if ( ch + 1 < line.length() && line[ch+1] == '>' )
			{
				binfo->fonts[ blocktextstart + drawntext.length() ] = SMALL_FONT_DIFF; // make the font larger
				ch += 1;
				continue;
			}

            // Vertical text alignment: @%T - top, @%M - middle, @%B - bottom
            if ( ch + 2 < line.length() && line[ch+1] == '%' )
			{
                switch ( line[ch+2] ) // .unicode()
                {
                    case 'T':
                        m_currentAlignment = VerticalTop;
                        break;

                    case 'M':
                        m_currentAlignment = VerticalMiddle;
                        break;

                    case 'B':
                        m_currentAlignment = VerticalMiddle;
                        break;
                }

                ch += 2;
                continue;
			}
		}

		// Nothing changes the color in title mode; also skip non-letters
        if ( !*intitle && isalnum(lchar) )
			timedcharacters.push_back( drawntext.length() );

		drawntext.push_back( lchar );
	}

	// Now we know the total number of characters, and can calc the time step
    if ( !timedcharacters.empty() )
	{
        int timestep = std::max( 1, (int) ((endtime - starttime) / timedcharacters.size() ) );

		for ( int ch = 0; ch < timedcharacters.size(); ch++ )
			binfo->offsets [ starttime + ch * timestep ] = blocktextstart + timedcharacters[ch];
	}

	binfo->text += drawntext;
	binfo->verticalAlignment = m_currentAlignment;
}*/

void TextRenderer::setRenderFont( const Font& font )
{
	m_renderFont = font;
	m_forceRedraw = true;
}

void TextRenderer::setColorBackground( const Color& color )
{
	m_colorBackground = color;
	m_forceRedraw = true;
}

void TextRenderer::setColorTitle( const Color& color )
{
	m_colorTitle = color;
	m_forceRedraw = true;
}

void TextRenderer::setColorToSing( const Color& color )
{
	m_colorToSing = color;
	m_forceRedraw = true;
}

void TextRenderer::setColorSang( const Color& color )
{
	m_colorSang = color;
	m_forceRedraw = true;
}

/*void TextRenderer::setTitlePageData( const std::string& artist, const std::string& title, const std::string& userCreatedBy, unsigned int msec )
{
	// setLyrics should be called before setTitlePageData
	if ( m_lyricBlocks.size() < 2 )
		abort();

    std::string createdBy = userCreatedBy;

    //createdBy = createdBy.replace( "<br>", "\n" );

	// Block 0 is reserved for us; fill it up
    std::string titletext = fmt::format("@#{:06x}{}\n\n{}\n\n{}", 0xffffff & m_colorTitle.rgb(), artist, title, createdBy);

	// Do we have at least 500ms to show the title?
    if ( m_lyricBlocks[1].timestart < 500 && !m_lyricBlocks[1].offsets.empty() )
		return;

	m_lyricBlocks[0].timestart = 0;
    m_lyricBlocks[0].timeend = std::min( (int64_t) msec, m_lyricBlocks[1].timestart );

	// Compile the line, replacing the spec characters
	bool intitle = true;
	compileLine( titletext, m_lyricBlocks[0].timestart, m_lyricBlocks[0].timeend, &m_lyricBlocks[0], &intitle );

	m_forceRedraw = true;
}*/

void TextRenderer::init()
{
	m_forceRedraw = true;

	m_preambleHeight = 0;
	m_preambleLengthMs = 0;
	m_preambleCount = 0;

	m_preambleTimeLeft = 0;
	m_lastDrawnPreamble = 0;
	m_lastSungTime = 0;
	m_drawPreamble = false;
	m_lastBlockPlayed = -2;
	m_lastPosition = -2;

	m_beforeDuration = 5000;
	m_afterDuration = 1000;
	m_prefetchDuration = 0;

    //m_lyricBlocks.clear();
}

/*uint TextRenderer::timingToGlyphCount(const Line &line, uint64_t timing) const {
    uint count = 0;
    if(timing < line.ca.starttime)
        return 0;
    if(timing > line.ca.endtime)
        return lyrics.font->getGlyphCount(line.ca.text);

    for(auto const& word: line.words) {
        for(auto const& syl: word.syllables) {
            if(syl.ca.endtime <= timing) {
                count += lyrics.font->getGlyphCount(syl.text);
            } else if(timing < syl.starttime) {
                count += 0;
            } else {
                double sungperc = static_cast<double>(timing - syl.starttime) / (syl.endtime - syl.starttime);
                count += static_cast<uint>(std::ceil(lyrics.font->getGlyphCount(syl.text) * sungperc));
            }
        }
        count++; //add a space between words
    }
    if(count == 0)
        exit(1);
    return count-1; // subtract the last space
}*/

void TextRenderer::renderBlock(const Block* currBlock, uint64_t timing, Image &image) const {
        std::vector<std::vector<std::pair<std::string, uint>>> lines;
        //std::vector<std::vector<std::string>> unsunglines;
        //std::vector<std::string> activelines;

        for (const auto& track: currBlock->tracks) {
            auto tracknr = (&track - &currBlock->tracks[0]);
            lines.push_back(std::vector<std::pair<std::string, uint>>());
            for(const auto& line: track.lines) {
                std::string strline = line.ca.text;
                uint sungcount = timingToGlyphCount(line, timing);
                lines.back().push_back(std::pair<std::string, uint>(strline, sungcount));
            }
        }


    image.fill(lyrics.bgcolor);
    uint64_t track_height = image.height() / lines.size();
    ssize_t lineheight = lyrics.font->getHeight();

    for(auto const& track: lines) {
        ssize_t track_nr = &track - &lines[0];
        image.setPen(lyrics.trackcolors[track_nr].first);

        size_t trackheight = track.size() * lineheight;
        size_t y = (track_height * track_nr) + ((track_height - trackheight) / 2);
        for(auto const& line: track) {
            ssize_t linewidth = lyrics.font->textWidth(line.first);
            size_t x = (image.width() - linewidth) / 2;
            image.drawText(x, y, line.first, *lyrics.font, line.second, lyrics.trackcolors[track_nr].second);
            y += lineheight;
        }
    }
}

void TextRenderer::renderTitleScreen(Image &image) const {
    // First get the dimensions of the generated text
    // so we can center it inside the image

    // We have the artists name, then a blank line, then the title.

    image.fill(lyrics.bgcolor);
    image.setPen(lyrics.titleColor);

    ssize_t height = lyrics.font->getHeight();

    ssize_t artist_width = lyrics.font->textWidth(lyrics.artist);
    ssize_t title_width = lyrics.font->textWidth(lyrics.title);

    // Now We can calculate the staring x and y coordinates
    ssize_t x_artist = (image.width() - artist_width) / 2;
    ssize_t x_title = (image.width() - title_width) / 2;
    ssize_t y = (image.height() - 3 * height) / 2;

    image.drawText(x_artist, y, lyrics.artist, *lyrics.font);
    image.drawText(x_title, y + 2 * height, lyrics.title, *lyrics.font);
    //image.save("/tmp/newo.png");
}

void TextRenderer::setPreambleData( unsigned int height, unsigned int timems, unsigned int count )
{
	m_preambleHeight = height;
	m_preambleLengthMs = timems;
	m_preambleCount = count;

	m_forceRedraw = true;
}

void TextRenderer::setDurations( unsigned int before, unsigned int after )
{
	m_beforeDuration = before;
	m_afterDuration = after;

	m_forceRedraw = true;
}

void TextRenderer::setPrefetch( unsigned int prefetch )
{
	m_prefetchDuration = prefetch;
	m_forceRedraw = true;
}

int TextRenderer::lyricForTime( int64_t tickmark, int * sungpos )
{
	int nextblk = -1;

	// Find the next playable lyric block
	for ( int bl = 0; bl < m_lyricBlocks.size(); bl++ )
	{
		if ( tickmark < m_lyricBlocks[bl].timestart )
		{
			nextblk = bl;
			break;
		}
	}

	// If there is a block within the prefetch timing, show it even if it overwrites the currently played block
	// (this is why this check is on top)
	if ( m_prefetchDuration > 0 && nextblk != -1 && m_lyricBlocks[nextblk].timestart - tickmark <= m_prefetchDuration )
	{
		// We update the timing, but the preamble show/noshow does not change here
        m_preambleTimeLeft = std::max( 0, (int) (m_lyricBlocks[nextblk].timestart - tickmark) );
		*sungpos = -1;
		return nextblk;
	}

	// Find the block which should be currently played, if any.
	int curblk = -1;
	int pos = -1;

	for ( int bl = 0; bl < m_lyricBlocks.size(); bl++ )
	{
		if ( tickmark < m_lyricBlocks[bl].timestart || tickmark > m_lyricBlocks[bl].timeend )
			continue;

		curblk = bl;

        //QMap< qint64, unsigned int >::const_iterator it = m_lyricBlocks[bl].offsets.find( tickmark );
        std::map<int64_t, unsigned int>::const_iterator it = m_lyricBlocks[bl].offsets.find( tickmark );

		if ( it == m_lyricBlocks[bl].offsets.end() )
            it = m_lyricBlocks[bl].offsets.lower_bound( tickmark );

		if ( it == m_lyricBlocks[bl].offsets.end() )
		{
			// This may happen if the whole block is title
			break;
		}

        pos = it->second;

		++it;
		break;
	}

	// Anything to play right now?
	if ( curblk == -1 )
	{
		// Nothing active to show, so if there is a block within next five seconds, show it.
		if ( nextblk != -1 && m_lyricBlocks[nextblk].timestart - tickmark <= m_beforeDuration )
		{
            m_preambleTimeLeft = std::max( 0, (int) (m_lyricBlocks[nextblk].timestart - tickmark) );

			// We show preamble if there was silence over PREAMBLE_MIN_PAUSE and the block
			// actually contains any time changes
            if ( tickmark - m_lastSungTime > PREAMBLE_MIN_PAUSE && !m_lyricBlocks[nextblk].offsets.empty() )
			{
				// m_preambleHeight == 0 disables the preamble
				if ( m_preambleHeight > 0 )
					m_drawPreamble = true;
			}

			*sungpos = -1;
			return nextblk;
		}

		// No next block or too far away yet
		m_drawPreamble = false;
		return -1;
	}

	m_drawPreamble = false;

	// If we're singing something (i.e. pos > 0), remember the timing
	if ( pos >= 0 )
		m_lastSungTime = tickmark;

	*sungpos = pos;
	return curblk;
}

/*bool TextRenderer::verifyFontSize( const QSize& size, const QFont& font )
{
	// Initialize the fonts
	QFont normalfont = font;

	// Test all lyrics whether it fits
	for ( int bl = 0; bl < m_lyricBlocks.size(); bl++ )
	{
		QRect rect = boundingRect( bl, normalfont );

		// Still fit?
		if ( rect.width() >= size.width() || rect.height() >= size.height() )
		{
			// Not fit, use a previous font size
			return false;
		}
	}

	return true;
}*/

/*int	TextRenderer::autodetectFontSize( const QSize& size, const QFont& font )
{
	QFont normalfont = font;

	// Start with 8
	int fontsize = 8;

	while ( 1 )
	{
		normalfont.setPointSize( fontsize );

		if ( !verifyFontSize( size, normalfont ) )
			return fontsize - 1;

		fontsize++;
	}
}*/

/*bool TextRenderer::checkFit( const QSize& imagesize, const QFont& font, const std::string& text )
{
	TextRenderer renderer(10,10);
	renderer.init();

	LyricBlockInfo testblock;
	testblock.timestart = 0;
	testblock.timeend = 0;

	bool intitle = true;
	renderer.compileLine( text, 0, 0, &testblock, &intitle );
	renderer.m_lyricBlocks.push_back( testblock );

	QRect rect = renderer.boundingRect( 0, font );

	return rect.width() <= imagesize.width() && rect.height() <= imagesize.height();
}*/


TextRenderer::TextSizes TextRenderer::boundingRect( size_t blockid, const Font& font )
{
    TextRenderer::TextSizes sizes;
    std::stringstream block(m_lyricBlocks[blockid].text);

	// Calculate the height
    //Font curFont(font);
    //QFontMetrics metrics = QFontMetrics( curFont );

	// Calculate the width and height for every line
    //ssize_t /*linewidth = 0, lineheight = 0,*/ totalheight = 0;
    //uint cur = 0;

    std::string line;

    while(std::getline(block, line)) {
        sizes.height += static_cast<size_t>(std::abs(font.getHeight()));
        size_t linewidth = static_cast<size_t>(std::abs(font.textWidth(line)));
        sizes.lineWidths.push_back(linewidth);
        sizes.maxWidth = std::max(sizes.maxWidth, linewidth);
    }

    /*while ( 1 )
	{
		// Line/text end
		if ( cur >= block.length() || block[cur] == '\n' )
		{
			// Adjust the total height
			totalheight += lineheight;            
            linewidths.push_back(linewidth);

			if ( cur >= block.length() )
				break; // we're done here

			cur++;
			linewidth = 0;

			continue;
        } */

		// We're calculating line width here, so check for any font change events
        //QMap< unsigned int, int >::const_iterator fontchange = m_lyricBlocks[blockid].fonts.find( cur );
        /*std::map< unsigned int, int >::const_iterator fontchange = m_lyricBlocks[blockid].fonts.find( cur );

		if ( fontchange != m_lyricBlocks[blockid].fonts.end() )
		{
            //curFont.setPointSize( curFont.pointSize() + fontchange->second );
            //metrics = QFontMetrics( curFont );
        }*/

        /*char prev = '\0';
        if(cur > 0) {
            prev = block[cur - 1];
        }*/

        /*ssize_t charwidth = curFont.getAdvance(block[cur], prev);
        size_t charheight =  curFont.getHeight();
        linewidth += charwidth;
        lineheight = std::max( lineheight, charheight );
        cur++;
    }*/

    return sizes;
}

void TextRenderer::drawLyrics( size_t blockid, int pos, const TextSizes& textSizes, Image& m_image )
{
    std::string block = m_lyricBlocks[blockid].text;
    //std::stringstream block(m_lyricBlocks[blockid].text);

	// Used in calculations only
    Font curFont( m_renderFont );
    Color fallbackColor = m_colorToSing;

	if ( pos == -1 )
        m_image.setPen( m_colorToSing );
	else
        m_image.setPen( m_colorSang );

	// Get the height offset from the rect.
    size_t start_y = 0;
	int verticalAlignment = m_lyricBlocks[blockid].verticalAlignment;

	// Draw title in the center, the rest according to the current vertical alignment
	if ( blockid == 0 || verticalAlignment == VerticalMiddle )
        start_y = (m_image.height() - textSizes.height) / 2 + m_renderFont.getHeight();
	else if ( verticalAlignment == VerticalTop )
        start_y = m_renderFont.getHeight() + m_image.width() / 50;	// see drawPreamble() for the offset
	else
        start_y = (m_image.height() - textSizes.height);

	// Draw the whole text
    uint linestart = 0;
    uint linenr = 0;
    size_t linewidth = 0;
    size_t cur = 0;

    std::string line;

    while(/*std::getline(block, line)*/ false) {
        auto glyphnr = m_image.loadText(line, &m_renderFont);
        size_t start_x = (m_image.width() - textSizes.lineWidths[linenr] ) / 2;
        for(uint i=0; i < glyphnr; i++) {
                std::map< unsigned int, std::string >::const_iterator colchange = m_lyricBlocks[blockid].colors.find( i );
                if ( colchange != m_lyricBlocks[blockid].colors.end() )
                {
                    Color newcolor( colchange->second );
                    fallbackColor = newcolor;

                    if ( i > pos )
                        m_image.setPen( newcolor );
                }

                if ( pos != -1 && i >= pos )
                {
                    m_image.setPen( fallbackColor );
                }
            if(cur > pos) {

            }
            auto advance = m_image.drawChar(start_x, start_y, i);
            cur++;
        }
        linenr++;
    }

	while ( 1 )
	{
        // Line/text end

		if ( cur >= block.length() || block[cur] == '\n' )
		{
			// Now we know the width, calculate the start offset
            size_t start_x = (m_image.width() - linewidth) / 2;

			// Draw the line
            for ( int i = linestart; i < cur; i++ )
			{
				// Handle the font change events
                //QMap< unsigned int, int >::const_iterator fontchange = m_lyricBlocks[blockid].fonts.find( i );
                std::map< unsigned int, int >::const_iterator fontchange = m_lyricBlocks[blockid].fonts.find( i );

				if ( fontchange != m_lyricBlocks[blockid].fonts.end() )
                {
                    uint32_t old_size  = curFont.pointSize();
                    curFont.setPointSize(old_size + fontchange->second);
                    //m_image.setFont( Font(m_image.font().family(), m_image.font().pointSize() + fontchange->second ) );
				}

				// Handle the color change events if pos doesn't cover them
                std::map< unsigned int, std::string >::const_iterator colchange = m_lyricBlocks[blockid].colors.find( i );

				if ( colchange != m_lyricBlocks[blockid].colors.end() )
				{
                    Color newcolor( colchange->second );
                    fallbackColor = newcolor;

					if ( i > pos )
                        m_image.setPen( newcolor );
				}

				if ( pos != -1 && i >= pos )
				{
                    m_image.setPen( fallbackColor );
				}

				// Outline
               /* const int OL = 0;
                Color oldPen = m_image.getPen();
                //painter.save();
                m_image.setPen( ~m_colorBackground.rgb() );//0xffffffff );
                m_image.drawText( start_x - OL, start_y - OL, block[i] );
                m_image.drawText( start_x + OL, start_y - OL, block[i] );
                m_image.drawText( start_x - OL, start_y + OL, block[i] );
                m_image.drawText( start_x + OL, start_y + OL, block[i] );
                //painter.restore();
                m_image.setPen(oldPen);*/

                m_image.drawText( start_x, start_y, /*block[i]*/ "TEST", curFont );
                //m_image.save("/tmp/foobar.png", "png");
                char prev = '\0';
                if(cur > 1) {
                    prev = block[cur - 1];
                }
                start_x += 0;//curFont.getAdvance(block[i], prev); //painter.fontMetrics().width( block[i] );
            }

			if ( cur >= block.length() )
				break; // we're done here

			// Start the next line
            start_y += curFont.getHeight();
			cur++;
			linewidth = 0;
			linestart = cur;
			continue;
		}

        /* TODO Font change // We're calculating line width here, so check for any font change events
        std::map< unsigned int, int >::const_iterator fontchange = m_lyricBlocks[blockid].fonts.find( cur );

		if ( fontchange != m_lyricBlocks[blockid].fonts.end() )
		{
            curFont.setPointSize( curFont.pointSize() + fontchange->second );
            //metrics = QFontMetrics( curFont );
        }*/
        char nextChar = '\0';
        if(cur + 1 < block.size()) {
            nextChar = block[cur + 1];
        }
        linewidth += 0;//curFont.getAdvance( block[cur], nextChar );
		cur++;
	}
}

void TextRenderer::drawPreamble(Image& m_image)
{
	// Is there anything to draw?
	if ( m_preambleTimeLeft <= PREAMBLE_SQUARE + 50 )
		return;

	int cutoff_time = m_preambleTimeLeft - PREAMBLE_SQUARE - 50;

    int preamble_spacing = m_image.width() / 100;
    int preamble_width = (m_image.width() - preamble_spacing * m_preambleCount ) / m_preambleCount;

    //QPainter painter( &m_image );
    m_image.setPen( 0xff000000 );
    m_image.setBrush( m_colorTitle );

	// Draw a square for each PREAMBLE_SQUARE; we do not draw anything for the last one, and speed up it 0.15sec
	for ( int i = 0; i < (int) m_preambleCount; i++ )
	{
		if ( i * PREAMBLE_SQUARE > cutoff_time )
			continue;

        m_image.drawRect( preamble_spacing + i * (preamble_spacing + preamble_width),
						  preamble_spacing,
						  preamble_width,
						  m_preambleHeight );
	}

	m_lastDrawnPreamble = m_preambleTimeLeft;
}


void TextRenderer::drawBackground( int64_t timing, Image& m_image )
{
	// Fill the image background
    m_image.fill( m_colorBackground.rgb() );

    if ( !m_lyricEvents->isEmpty() ) {
    m_lyricEvents->draw( timing, m_image );
    }
}

int TextRenderer::myupdate(size_t timing, Image &image) {
    //find block
    if(timing < 1000) {
        renderTitleScreen(image);
        return 0;
    }
    const Block* currBlock = lyrics.blockByTiming(timing);

    if(currBlock != nullptr) {
        renderBlock(currBlock, timing, image);
        return 0;
    }
    image.fill(lyrics.bgcolor);
    return 0;
}

int TextRenderer::update( int64_t timing, Image& m_image )
{
	int result = UPDATE_COLORCHANGE;
	bool redrawPreamble = false;
	int sungpos = 0;

	// Should we show the title?
	int blockid = lyricForTime( timing, &sungpos );

	// Force the full redraw if time went backward
	if ( timing < m_lastSungTime )
		m_forceRedraw = true;

	// Is it time to redraw the preamble?
	if ( m_drawPreamble && abs( m_lastDrawnPreamble - m_preambleTimeLeft ) > 450 )
		redrawPreamble = true;

    // Check whether we can skip the redraws
    bool background_updated = (m_lyricEvents->isEmpty() || !m_lyricEvents->updated( timing )) ? false : true;

	if ( !m_forceRedraw && !redrawPreamble && !background_updated )
	{
		// Did lyrics change at all?
		if ( blockid == m_lastBlockPlayed && sungpos == m_lastPosition )
			return UPDATE_NOCHANGE;

		// If the new lyrics is empty, but we just finished playing something, keep it for 5 more seconds
		// (i.e. post-delay)
		if ( blockid == -1 && timing - m_lastSungTime < 5000 )
			return UPDATE_NOCHANGE;
	}

	// Draw the background first
    drawBackground( timing, m_image );

	// Did we get lyrics?
	if ( blockid != -1 )
	{
		// Do the new lyrics fit into the image without resizing?
        auto textSizes = boundingRect( blockid, m_renderFont );

        if ( textSizes.maxWidth > m_image.width() || textSizes.height > m_image.height() )
		{
            m_image.resize( std::max( textSizes.maxWidth + 10, m_image.width() ),
                                   std::max( textSizes.height + 10, m_image.height() ) );
			result = UPDATE_RESIZED;

			// Draw the background again on the resized image
            drawBackground( timing, m_image );
		}

		// Draw the lyrics
        drawLyrics( blockid, sungpos, textSizes, m_image );

		// Draw the preamble if needed
		if ( m_drawPreamble )
            drawPreamble(m_image);
	}

	// Is the text change significant enough to warrant full screen redraw?
	if ( blockid != m_lastBlockPlayed || m_forceRedraw )
	{
		if ( result != UPDATE_RESIZED )
			result = UPDATE_FULL;
	}

	//saveImage();

	m_lastBlockPlayed = blockid;
	m_lastPosition = sungpos;

	m_forceRedraw = false;
	return result;
}


void TextRenderer::prepareEvents()
{
    m_lyricEvents->prepare();

	// Events need to be adjusted for the following:
	// - Those at the start of the block may move;
    int64_t lastend = 0;

    //for ( int i = 0; i < m_lyricBlocks.size(); i++ )
    for(const auto& m_lyricBlock : m_lyricBlocks)
	{
        const int64_t start = m_lyricBlock.timestart;
        const int64_t end = m_lyricBlock.timeend;

		// Should the next block be shown earlier than expected via prefetch?
		if ( start - lastend > m_beforeDuration )
		{
			// Adjust by m_beforeDuration
            m_lyricEvents->adjustTime( start, start - m_beforeDuration );
		}
		else if ( m_prefetchDuration > 0 )
		{
			// Adjust by m_prefetchDuration
            m_lyricEvents->adjustTime( start, start - m_prefetchDuration );
		}
		else if ( start > lastend )
		{
			// Adjust by start - lastend
            m_lyricEvents->adjustTime( start, start - (start - lastend) );
		}

		lastend = end;
	}
}
