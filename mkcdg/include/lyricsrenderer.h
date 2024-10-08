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

#ifndef LYRICSRENDERER_H
#define LYRICSRENDERER_H

#include "image.h"
#include <memory>

// This is an abstract rendering class which covers CD+G and text rendering
class LyricsRenderer
{
	public:

		LyricsRenderer();
		virtual ~LyricsRenderer();

        virtual int	update( int64_t timing ) = 0;
        Image*	image() const;

	protected:
		// Rendered image
        Image*	m_image;
};

#endif // LYRICSRENDERER_H
