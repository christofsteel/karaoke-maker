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

#ifndef LYRICSEVENTS_H
#define LYRICSEVENTS_H

#include <string>
#include <map>
#include <memory>

#include "color.h"
#include "image.h"

class Background;

class LyricsEvents
{
	public:
		LyricsEvents();
		~LyricsEvents();

		// copy
		LyricsEvents( const LyricsEvents& );

		// check
		bool isEmpty() const;

		// Add the events
        bool addEvent( int64_t timing, const std::string& text );

		// When playing
        bool prepare(/* std::string * errmsg = 0 */);
        void adjustTime( int64_t timing, int64_t newtiming );
        bool updated( int64_t timing ) const;
        void draw( int64_t timing, Image& image );

        //static std::string validateEvent( const std::string& text );

	private:
		class Event
		{
			public:
                int64_t		timing;
				int			type;
                std::string		data;
		};

		void	cleanPrepared();
        static bool parseEvent( const std::string& text, Event& event/*, std::string * errmsg = 0 */);

		// Event storage which is copied
        std::map< int64_t, Event > m_events;		

		// Prepared event storage which is NOT copied
        std::map< int64_t, std::shared_ptr<Background> > m_preparedEvents;

		// State
        Image*	m_cachedImage;
        int64_t		m_lastUpdate;
        int64_t		m_nextUpdate;

		// Current event timing
        int64_t		m_eventTiming;
};

#endif // LYRICSEVENTS_H
