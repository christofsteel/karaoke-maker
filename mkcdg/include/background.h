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

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <iostream>
#include <string>
#include <memory>
#include "color.h"
#include "image.h"

class Background
{
	public:
        Background(const std::string& filename);
        Background(int width, int height, const Color& color = Color(0xff000000));
        Background();
        ~Background();

        void load(const std::string& filename);
        void fill(const Color& color);

		// Actual draw function to implement. Should return time when the next
		// doDraw() should be called - for example, for videos it should only
		// be called when it is time to show the next frame; for static images
		// it should not be called at all. If 0 is returned, doDraw() will be called
		// again the next update. If -1 is returned, doDraw() will never be called
		// again, and the cached image will be used.
        int64_t doDraw(Image* image);

		// Should return true if the event was created successfully
        bool isValid() const;        
private:
    Image	m_image;
    bool valid;

};

#endif // BACKGROUND_H
