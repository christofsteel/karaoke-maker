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

#include <iostream>
#include <fstream>
#include <algorithm>

#include <fmt/format.h>

#include "cdggenerator.h"


// Color code indexes
static int COLOR_IDX_BACKGROUND = 0;	// background

/*CDGGenerator::CDGGenerator(
        const std::string& outputFile,
        const Color& colorBackground,
        const Color& colorInfo,
        const Color& colorInactive,
        const Color& colorActive,
        bool antiAlias,
        const std::string& fontName,
        uint32_t fontSize,
        const std::string& artist,
        const std::string& title,
        const std::string& createdBy,
        size_t titleTime
        )
{
    m_outputFile = outputFile;
    m_colorBackground = colorBackground;
    m_colorInfo = colorInfo;
    m_colorInactive = colorInactive;
    m_colorActive = colorActive;
    m_renderFont = Font(fontName, fontSize, antiAlias);
    m_enableAntiAlias = antiAlias;
    m_artist = artist;
    m_title = title;
    m_createdBy = createdBy;
    m_titleTime = titleTime;
}*/

CDGGenerator::CDGGenerator(const std::string& outputfile, XMLLyrics& lyrics) : m_lyrics(lyrics){
    m_outputFile = outputfile;

    // Gather the defined colors
    // If more than 16 colors are defined in the lyricsfile, we take only the first 16
    addColor(lyrics.borderColor);
    addColor(lyrics.titleColor);
    for(const auto& block: lyrics.blocks) {
        if(block.ca.bgtype == BG_COLOR)
            addColor(Color(block.ca.background));
        for(const auto& track: block.tracks) {
            for(const auto& line: track.lines) {
                for(const auto& word: line.words) {
                    for(const auto& syl: word.syllables) {
                        addColor(syl.ca.sung_color);
                        addColor(syl.ca.unsung_color);
                    }

                }

            }

        }
    }
    m_streamColorIndex = -1;
    addEmpty();
    clearScreen();
}

void CDGGenerator::addColor(const Color& color) {
    if((m_colors.size() < 16) && (std::find(m_colors.begin(), m_colors.end(), color) == m_colors.end())) {
            m_colors.push_back(color);
    }
}



void CDGGenerator::addColorGradations( const Color& color, unsigned int number )
{
    int r = color.red();
    int g = color.green();
    int b = color.blue();

	int r_step = r / number;
	int g_step = g / number;
	int b_step = b / number;

	for ( unsigned int i = 0; i < number; i++ )
	{
        Color newcolor( r - r_step * i, g - g_step * i, b - b_step * i );
        m_colors.push_back( newcolor );
	}
}

int CDGGenerator::getColor( Rgb rgbcolor )
{
	// See http://stackoverflow.com/questions/4057475/rounding-colour-values-to-the-nearest-of-a-small-set-of-colours
    Color targetcolor( rgbcolor | 0xff000000 );

	int smallest_dist_idx = -1;
    double smallest_dist = HUGE_VAL;

	// Calculate the smallest color distance between this color and other colors in the array
    for ( size_t i = 0; i < m_colors.size(); i++ )
	{
        const Color& origcolor = m_colors.at( i );

		double dist = sqrt( pow( origcolor.redF() - targetcolor.redF(), 2.0)
							+ pow( origcolor.greenF() - targetcolor.greenF(), 2.0 )
							+ pow( origcolor.blueF() - targetcolor.blueF(), 2.0 ) );

		if ( smallest_dist_idx == -1 || dist < smallest_dist )
		{
			smallest_dist_idx = i;
			smallest_dist = dist;
		}
	}

    // If we have room in the color table and the distance is too big, add it
    if ( m_colors.size() < 16 && smallest_dist > 0.3 )
    {
        m_colors.push_back( targetcolor );
        smallest_dist_idx = m_colors.size() - 1;
    }

	return smallest_dist_idx;
}

void CDGGenerator::addSubcode( const SubCode& sc )
{
    m_stream.push_back( sc );
}

void CDGGenerator::addEmpty()
{
	SubCode sc;
	memset( &sc, 0, sizeof( sc ) );

	addSubcode( sc );
}

void CDGGenerator::fillColor( char * buffer, const Color& color )
{
	// Green
    char red = (color.red() / 17) & 0x0F;
    char green = (color.green() / 17) & 0x0F;
    char blue = (color.blue() / 17) & 0x0F;

	// Red and green
	buffer[0] = (red << 2) | (green >> 2);
	buffer[1] = ((green & 0x03) << 5 ) | blue;
}

void CDGGenerator::clearScreen()
{
    SubCode sc;

	// First copy the colors if we got them (there is always one background color)
	if ( m_streamColorIndex != -1 && m_colors.size() > 1 )
	{
		// Load first lower 8 colors
		SubCode sc;

		sc.command = CDG_COMMAND;
		sc.instruction = CDG_INST_LOAD_COL_TBL_0_7;
		memset( sc.data, 0, 16 );

        for ( size_t i = 0; i < 7; i++ )
		{
			if ( i >= m_colors.size() )
				break;

			fillColor( sc.data + i * 2, m_colors[i] );
		}

		m_stream[ m_streamColorIndex ] = sc;

		// Do we have more colors?
        if ( m_colors.size() > 8 )
		{
            sc.instruction = CDG_INST_LOAD_COL_TBL_8_15;
			memset( sc.data, 0, 16 );

            for ( size_t i = 8; i < std::min(static_cast<size_t>(16), m_colors.size()); i++ )
			{
                fillColor( sc.data + (i - 8) * 2, m_colors[i] );
			}

			m_stream[ m_streamColorIndex + 1 ] = sc;
        }

		//initColors();
	}

	// Now clear the screen
	for ( int i = 0; i < 16; i++ )
	{
        SubCode sc;
        sc.command = CDG_COMMAND;
		sc.instruction = CDG_INST_MEMORY_PRESET;
        CDG_MemPreset* preset = reinterpret_cast<CDG_MemPreset*>(sc.data);

		preset->repeat = i & 0x0F;
        preset->color = static_cast<char>(COLOR_IDX_BACKGROUND);

		addSubcode( sc );
	}

    sc.command = CDG_COMMAND;
	sc.instruction = CDG_INST_BORDER_PRESET;
    CDG_BorderPreset* preset = reinterpret_cast<CDG_BorderPreset*>(sc.data);

    preset->color = static_cast<char>(COLOR_IDX_BACKGROUND);    

	addSubcode( sc );

	// Reserve space for two LoadColor commands
    m_streamColorIndex = static_cast<int>(m_stream.size());
	addEmpty();
	addEmpty();
}

std::vector<char> CDGGenerator::stream()
{
    //return std::vector<char>(m_stream.begin(), m_stream.end());

    //slow, but correct (I think)
    std::vector<char> return_vec;
    for(const auto& subcode: m_stream) {
        return_vec.push_back(subcode.command);
        return_vec.push_back(subcode.instruction);
        return_vec.push_back(subcode.parityQ[0]);
        return_vec.push_back(subcode.parityQ[1]);
        for(int i = 0; i < 16; i++) {
            return_vec.push_back(subcode.data[i]);
        }
        return_vec.push_back(subcode.parityP[0]);
        return_vec.push_back(subcode.parityP[1]);
        return_vec.push_back(subcode.parityP[2]);
        return_vec.push_back(subcode.parityP[3]);
    }
    return return_vec;


}


void CDGGenerator::checkTile( int offset_x, int offset_y, const Image* orig, Image* newimg )
{
	// The first loop checks if there are any colors to change, and enumerates them
    std::map< int, std::vector<int> > color_changes;

	// Tiles are 6x12
	for ( int y = 0; y < 12; y++ )
	{
		// Since the offsets assume borders, but our image does not contain them, we
		// adjust in the calculations
        size_t image_offset_y = y + offset_y - CDG_BORDER_HEIGHT;

        const Color* orig_line = orig->scanLine( image_offset_y );
        const Color*  new_line = newimg->scanLine( image_offset_y );

		for ( int x = 0; x < 6; x++ )
		{
			int image_offset_x = offset_x + x - CDG_BORDER_WIDTH;

            if ( orig_line[ image_offset_x ].rgb() == new_line[ image_offset_x ].rgb() )
				continue;

            int origcolor = getColor( orig_line[ image_offset_x ].rgb() );
            int newcolor = getColor( new_line[ image_offset_x ].rgb() );

			// Calculate the mask for the color change
			int mask = origcolor ^ newcolor;

            if ( (mask & 0xFFFFFF00) != 0 )
                std::cerr << "error in mask calculation" << std::endl;

			// Store the coordinates in lo/hi bytes
			int coord = x << 8 | y;
			color_changes[ mask ].push_back( coord );
		}
	}

	// Anything to change?
    if ( color_changes.empty() )
		return;

	// Enumerate the map entries
    std::vector<char> colors;
    for(const auto& [key, val]: color_changes) {
        colors.push_back(key);
    }

	// Bitmasks
    uint8_t bitmask[6] = { 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };

    for ( size_t i = 0; i < colors.size(); i++ )
	{
		SubCode sc;

		sc.command = CDG_COMMAND;
		sc.instruction = CDG_INST_TILE_BLOCK_XOR;
		memset( sc.data, 0, 16 );
        CDG_Tile* tile = reinterpret_cast<CDG_Tile*>(sc.data);

        tile->column = static_cast<char>(offset_x / 6);
        tile->row = static_cast<char>(offset_y / 12);
		tile->color0 = 0;
		tile->color1 = colors[ i ];

        const std::vector<int>& coords = color_changes[ colors[ i ] ];

        for ( size_t p = 0; p < coords.size(); p++ )
		{
			int x = coords[p] >> 8;
			int y = coords[p] & 0x0F;

			tile->tilePixels[y] |= bitmask[x];
		}

		addSubcode( sc );
	}
}

void CDGGenerator::applyTileChanges( const Image& orig, Image* newimg )
{
	// Tiles are 6x12, but we skip the border area 
	for ( unsigned int offset_y = CDG_BORDER_HEIGHT; offset_y < CDG_FULL_HEIGHT - CDG_BORDER_HEIGHT; offset_y += 12 )
		for ( unsigned int offset_x = CDG_BORDER_WIDTH; offset_x < CDG_FULL_WIDTH - CDG_BORDER_WIDTH; offset_x += 6 )
            checkTile( offset_x, offset_y, &orig, newimg );
}

void CDGGenerator::myGenerate() {
    // Prepare images
    Image* lastImage = new Image ( CDG_DRAW_WIDTH, CDG_DRAW_HEIGHT);//, QImage::Format_ARGB32 );
    Image* currImage = new Image ( CDG_DRAW_WIDTH, CDG_DRAW_HEIGHT);//, QImage::Format_ARGB32 );
    clearScreen();
    lastImage->fill( m_lyrics.borderColor);

    Mytextrenderer renderer(CDG_DRAW_WIDTH, CDG_DRAW_HEIGHT, m_lyrics);

    size_t total_length = m_lyrics.ca.endtime;

    m_stream.reserve( total_length * 300 / 1000 );

    size_t timing = 0;

    while(timing < total_length) {
        size_t currSize = m_stream.size();
        timing = currSize * 1000 / 300 + 250;

        auto status = renderer.update(timing, *currImage);
        if(status == NEW_BLOCK) {
            currImage->save("/tmp/cur.png");
            lastImage->save("/tmp/last.png");
            clearScreen();
            lastImage->fill(m_lyrics.borderColor);
        } else if(status == NO_CHANGE) {
            addEmpty();
            continue;
        }
        //currImage->save("/tmp/img.png");

        applyTileChanges( *lastImage, currImage );
        *lastImage = *currImage;
        if(currSize == m_stream.size()) {
            addEmpty();
        }
        /*if(timing < 8100 && timing > 6999) {
            currImage->save(fmt::format("/tmp/{}.png", timing));
        }*/
    }
    clearScreen();
    try {
        char *p = (char*) &m_stream[0];

        for ( unsigned int i = 0; i < m_stream.size() * sizeof(SubCode); i++, p++ ) {
            if ( *p & 0xC0 )
                *p &= 0x3F;
        }

        std::ofstream file(m_outputFile, std::ios::binary);

        if ( !file ) {
            std::cerr << "Cannot write CD+G file " << m_outputFile << std::endl;// << ": " << file.errorString().toStdString() << std::endl;
            return;
        }

        const std::vector<char>& data = stream();

        file.write( &data[0], data.size() );
        file.close();
    }
    catch ( std::string& txt ) {
        std::cerr << "Cannot write CD+G file: " << txt << std::endl;
        return;
    }
}

/*void CDGGenerator::generate( XMLLyrics& lyrics, size_t total_length )
{
	// Initialize the buffer and colors
	init();

    // Prepare images
    Image* lastImage = new Image ( CDG_DRAW_WIDTH, CDG_DRAW_HEIGHT);//, QImage::Format_ARGB32 );
    Image* currImage = new Image ( CDG_DRAW_WIDTH, CDG_DRAW_HEIGHT);//, QImage::Format_ARGB32 );
    lastImage->fill( m_colorBackground );


	// Prepare the renderer
    TextRenderer lyricrenderer( CDG_DRAW_WIDTH, CDG_DRAW_HEIGHT);

    // Lyrics must be set before anything else as it overrides the data
    lyricrenderer.setLyrics( lyrics );

    lyricrenderer.setRenderFont( m_renderFont );

    // Colors
    lyricrenderer.setColorBackground( m_colorBackground );
    lyricrenderer.setColorTitle( m_colorInfo );
    lyricrenderer.setColorSang( m_colorInactive );
    lyricrenderer.setColorToSing( m_colorActive );

	// Title
    lyricrenderer.setTitlePageData( m_artist,
                                    m_title,
                                    m_createdBy,
                                    m_titleTime );

	// Preamble
	lyricrenderer.setPreambleData( 4, 5000, 8 );

	// CD+G prefetching
	lyricrenderer.setPrefetch( 1000 );

    if(total_length == 0) {
        total_length = lyricrenderer.getLastTime();
    }

    // Preallocate the arrays
	init();
	m_stream.reserve( total_length * 300 / 1000 );

	// Render
	try
	{
		while ( 1 )
        {
			// There should be 300 packets in 1000ms of music
			// So each packet adds 1000 / 300 ms
			// Speed up time a little to compensate CD+G reader delay
            int64_t timing = m_stream.size() * 1000 / 300 + 250;
            std::cout << timing << "/" << total_length << "\r";

			if ( timing > total_length )
                break;

            int status = lyricrenderer.update( timing, *currImage );

            if ( status == TextRenderer::UPDATE_RESIZED )
			{
                currImage->save( "error", "bmp" );
                std::cerr << fmt::format("Lyrics out of boundary at {}, screen requested: {}x{}",
                                         timing, currImage->width(), currImage->height()) << std::endl;

				m_stream.clear();
				return ;
			}

            if ( status == TextRenderer::UPDATE_NOCHANGE )
			{
				addEmpty();
				continue;
			}

			// Is change significant enough to warrant full redraw?
            if ( status == TextRenderer::UPDATE_FULL )
			{
				clearScreen();

				// Clear the old image too
                lastImage->fill( m_colorBackground.rgb() );
			}

			int packets = m_stream.size();
//            currImage = lyricrenderer.image();
//            currImage->save("/tmp/img.png", "png");
            applyTileChanges( *lastImage, currImage );
            *lastImage = *currImage;

			// Make sure we added at least some tiles
            if ( packets == m_stream.size() )
				addEmpty();
		}

         delete lastImage;
         delete currImage;
        // Clean up the parity bits in the CD+G stream
		char *p = (char*) &m_stream[0];

		for ( unsigned int i = 0; i < m_stream.size() * sizeof(SubCode); i++, p++ )
		{
			if ( *p & 0xC0 )
				*p &= 0x3F;
		}

        std::ofstream file(m_outputFile, std::ios::binary);

        if ( !file )
		{
            std::cerr << "Cannot write CD+G file " << m_outputFile << std::endl;// << ": " << file.errorString().toStdString() << std::endl;
			return;
		}

        const std::vector<char>& data = stream();

        file.write( &data[0], data.size() );
        file.close();
	}
    catch ( std::string& txt )
    {
        std::cerr << "Cannot write CD+G file: " << txt << std::endl;
		return;
	}
} */
