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

#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <iostream>
#include <map>

#include "lyricsrenderer.h"
#include "lyricsevents.h"
#include "xmllyrics.h"
#include "image.h"
#include "font.h"

class TextRenderer
{
	public:
        enum
        {
            UPDATE_NOCHANGE,	// image did not change at all
            UPDATE_COLORCHANGE,	// only colors of some image characters changed
            UPDATE_FULL,		// a whole image changed
            UPDATE_RESIZED,		// not only the whole image changed, but also the image size changed
        };
        TextRenderer( int width, int height, const XMLLyrics& lyrics );
        ~TextRenderer() {
            //delete m_image;
        }

        size_t getLastTime();

		// Sets the lyrics to render. Resets all previously set params to defaults except setDisplaySize.
        void	setLyrics( XMLLyrics& lyrics );

		// Lyrics data to render, overrides defaults from settings
        void	setRenderFont( const Font& font);
        void	setColorBackground( const Color& color );
        void	setColorTitle( const Color& color );
        void	setColorToSing( const Color& color );
        void	setColorSang( const Color& color );
		void	setPreambleData( unsigned int height, unsigned int timems, unsigned int count );
        void	setTitlePageData( const std::string& artist, const std::string& title, const std::string& userCreatedBy, unsigned int msec ); // duration = 0 - no title, default		

		// Typically lyrics are shown a little before they are being sung, and kept after they end.
		// This function overrides default before (5000ms) and after (1000ms) lengths
		void	setDurations( unsigned int before, unsigned int after );

		// Some formats (like CD+G) draw lyrics pretty slow, which means if the lyrics screens
		// change immediately, at the time the next page is being drawn it is already sung.
		// This parameter changes the prefetch, meaning if its value is 1000, the new lyrics block
		// is always shown at least 1000ms before it is being sing, even if it is necessary to cut the
		// old block earlier. Default is zero.
		void	setPrefetch( unsigned int prefetch );

		// Draw a new lyrics image
        int	update( int64_t timing, Image& m_image );

        int	myupdate( size_t timing, Image& image );
		// Checks if a line or block fits into the requested image.
        //static	bool checkFit( const Size& imagesize, const std::string fontname, int fontsize , const std::string& text );

		// Autodetects the largest font size to fit all lyrics into a specific image size.
        //int		autodetectFontSize( const QSize& imagesize, const QFont& font );

		// Verifies that all lyrics could be rendered into a specific image size using the provided font
        //bool	verifyFontSize( const QSize& imagesize, const QFont& font );

		enum VerticalAlignment
		{
			// default = 0
			VerticalBottom = 0,
			VerticalMiddle = 1,
			VerticalTop = 2
		};

        struct TextSizes {
                size_t height;
                size_t maxWidth;
                std::vector<size_t> lineWidths;
        };

	private:
		// Returns the lyrics bounding box for a line or for paragraph using the font specified,
		// or the default font if not specified
        TextSizes	boundingRect( size_t blockid, const Font& font );

        const XMLLyrics& lyrics;
		void	init();
		void	prepareEvents();
        int		lyricForTime( int64_t tickmark, int * sungpos );
        void    renderTitleScreen(Image& image) const;
        void    renderBlock(const Block* currBlock,uint64_t timing, Image& image) const;
        uint    timingToGlyphCount(const Line& line, uint64_t timing) const;
        void	fixActionSequences( std::string& block );
        void	drawLyrics( size_t blockid, int pos, const TextSizes& boundingRect, Image& m_image );
        void	drawPreamble(Image& m_image);
        void	drawBackground( int64_t timing, Image& m_image );

	private:
		// Lyrics to render
        typedef struct
		{
            int64_t	timestart;
            int64_t	timeend;

			// Text is for the whole block, with all special characters stripped down
            std::string	text;

			// Text offsets in block per specific time
            std::map< int64_t, unsigned int > offsets;

			// Per-character color changes for following (non-sung) characters in the block.
			// if none, the default color is used
            std::map< unsigned int, std::string > colors;

			// Per-character font size changes for following (non-sung) characters in the block.
			// if none, the default color is used
            std::map< unsigned int, int > fonts;

			int	verticalAlignment;

		} LyricBlockInfo;

        std::vector< LyricBlockInfo >	m_lyricBlocks;

		// Compile a single line
        void	compileLine( const Line& line, /*int64_t starttime, int64_t endtime, LyricBlockInfo * binfo,*/ bool *intitle );

		// True if the image must be redrawn even if lyrics didn't change
		bool					m_forceRedraw;

		// Rendering params
        Color					m_colorBackground;
        Color					m_colorTitle;
        Color					m_colorToSing;
        Color					m_colorSang;
        Font					m_renderFont;
		unsigned int			m_preambleHeight;	// how tall the preamble square should be; 0 - no preamble
		unsigned int			m_preambleLengthMs;	// maximum time the preamble is shown
		unsigned int			m_preambleCount;	// how many preamble squares to draw for m_preambleLengthMs
		unsigned int			m_beforeDuration;
		unsigned int			m_afterDuration;
		unsigned int			m_prefetchDuration;

		// Handling the preamble stuff
		int						m_preambleTimeLeft;	// Time left to show the current preamble - 5000 ... 0
		int						m_lastDrawnPreamble; // Timing when the last time the preamble changed
        int64_t					m_lastSungTime;
		bool					m_drawPreamble;

		int						m_lastBlockPlayed;
		int						m_lastPosition;

		// Background events
        LyricsEvents*			m_lyricEvents;

		// Vertical alignment
		int						m_currentAlignment;
};

#endif // TEXTRENDERER_H
