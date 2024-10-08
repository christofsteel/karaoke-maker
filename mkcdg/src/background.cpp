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

#include "background.h"

Background::Background(const std::string& filename ) : m_image(filename) {
    valid = true;
/*        if ( !m_image->load(filename) ) {
            std::cerr << "Cannot load image file " << filename << std::endl;
valid = false;
        }*/
}

Background::Background(int width, int height, const Color& color) : m_image(width,height) {
    m_image.fill(color);
    valid = true;
}

/*Background::Background() {
    //m_image->fill(Color(0xff000000));
    m_image = std::make_shared<Image>(Color(0xff000000));
    valid = true;
}*/

void Background::fill(const Color& color) {    
    m_image.fill(color);
    valid = true;
}

void Background::load(const std::string& arg) {
    valid = true;
    if ( !m_image.load(arg ) ) {
        std::cerr << "Cannot load image file " << arg << std::endl;
        valid = false;
    }
}

Background::~Background()
{
}


bool Background::isValid() const
{
    return valid;//!m_image.isNull();
}

int64_t Background::doDraw( Image* image)
{
	// We don't care about timing
    *image = m_image;

	// No updates
	return -1;
}
