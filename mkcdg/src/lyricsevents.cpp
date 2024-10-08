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

#include <regex>

#include <fmt/format.h>
#include "lyricsevents.h"
#include "background.h"
#include "util.h"

enum
{
    TYPE_DEFAULT,
	TYPE_IMAGE,
    TYPE_COLOR,
};

LyricsEvents::LyricsEvents()
{
	m_lastUpdate = -1;
	m_nextUpdate = 0;
	m_eventTiming = 0;
    m_cachedImage = new Image();
}

LyricsEvents::~LyricsEvents()
{
	cleanPrepared();
    delete m_cachedImage;
}

LyricsEvents::LyricsEvents( const LyricsEvents& ev )
{
	// We do not copy m_preparedEvents and m_cachedImage
	m_events = ev.m_events;

	m_lastUpdate = -1;
	m_nextUpdate = 0;
	m_eventTiming = 0;
    m_cachedImage = new Image();
}

bool LyricsEvents::addEvent( int64_t timing, const std::string& text )
{
	Event ev;

    if ( !parseEvent( text, ev ) )
		return false;

	ev.timing = timing;
	m_events[ timing ] = ev;
	return true;
}

bool LyricsEvents::isEmpty() const
{
    return m_events.empty();
}

bool LyricsEvents::parseEvent( const std::string& text, Event& event/*, std::string * errmsg */)
{
    std::regex check("^(\\w+)=(.*)$");
    std::string key, value;
    std::smatch match;
    std::string trimmed_text = trim(text);

    if ( trimmed_text != "DEFAULT" )
    {
        if ( !std::regex_match(trimmed_text, match, check) ) {
            /*if(errmsg != nullptr) {
                *errmsg = "Invalid event format; must be like IMAGE=path"; */
                return false;
            //}
        }
        key = match[1];
        value = match[2];
    }
    else
        key = trimmed_text;

	if ( key == "IMAGE" )
	{
        Image img;

        if ( !img.load( value ) )
		{
            /*if ( errmsg )
                *errmsg = fmt::format("File {} is not a supported image", value); */

			return false;
		}

                    event.type = TYPE_IMAGE;
            event.data = value;


		return true;
	}
    else if ( key == "DEFAULT" )
    {

            event.type = TYPE_DEFAULT;
            event.data.clear();


        return true;
    }
    else if ( key == "COLOR" )
    {
        /*if ( !QColor::isValidColor(QString(value.c_str())) )
        {
            if ( errmsg )
                *errmsg = fmt::format("Color {} is not valid", value);

            return false;
        }*/


            event.type = TYPE_COLOR;
            event.data = value;


        return true;
    }

    /*if ( errmsg )
        *errmsg = fmt::format("Invalid event name '{}'", key); */

	return false;
}

void LyricsEvents::cleanPrepared()
{
//	for ( std::map< int64_t, Background* >::iterator it = m_preparedEvents.begin(); it != m_preparedEvents.end(); ++it )
    //for( auto const& [key, bg] : m_preparedEvents )
    //    delete bg;

	m_preparedEvents.clear();
}

bool LyricsEvents::prepare(/* std::string * errmsg */)
{
	cleanPrepared();

    //for ( QMap< qint64, Event >::const_iterator it = m_events.begin(); it != m_events.end(); ++it )
    for( auto const& [key, ev] : m_events )
    {
        std::shared_ptr<Background> bgev = nullptr;

        switch ( ev.type )
		{
			case TYPE_IMAGE:
                bgev = std::make_shared<Background>(ev.data);
                //bgev->m_image->save("/tmp/bg.png", "png");
				break;

            case TYPE_COLOR:
                bgev = std::make_shared<Background>(288, (216 - 24),Color(ev.data)); //TODO
                break;

            case TYPE_DEFAULT:                
                break;

            default:
                continue;
		}


            if ( bgev != nullptr && !bgev->isValid() )
            {      
                /*if ( errmsg )
                    *errmsg = "Invalid event";*/
                bgev = nullptr;

                continue;
            }


        m_preparedEvents[ key ] = bgev;
	}

	return true;
}

void LyricsEvents::adjustTime( int64_t timing, int64_t newtiming )
{
    std::map< int64_t, std::shared_ptr<Background> >::iterator it = m_preparedEvents.find( timing );

	if ( it != m_preparedEvents.end())
	{
		m_preparedEvents[ newtiming ] = m_preparedEvents[timing];
		m_preparedEvents.erase( it );
	}
}

bool LyricsEvents::updated( int64_t timing ) const
{
	if ( m_nextUpdate == -1 )
		return false;

	if ( m_nextUpdate == 0 )
		return true;

	return ( m_nextUpdate <= timing );
}

void LyricsEvents::draw( int64_t timing, Image& image )
{
	// Do we have precompiled events?
    if ( !m_events.empty() && m_preparedEvents.empty() )
		return;

	bool cache_changed = false;

	// Find current event
    std::map< int64_t, std::shared_ptr<Background> >::const_iterator found = m_preparedEvents.end();

    for ( std::map< int64_t, std::shared_ptr<Background> >::const_iterator it = m_preparedEvents.begin(); it != m_preparedEvents.end(); ++it )
    {
        if ( it->first <= timing )
            found = it;
	}

	if ( found == m_preparedEvents.end() )
		return;

    std::shared_ptr<Background> bg = found->second;



	// Same event as before?
    if ( found->first != m_eventTiming )
	{
        m_eventTiming = found->first;
		cache_changed = true;	
	}

	m_lastUpdate = timing;

    if(bg == nullptr) {
        m_nextUpdate = -1;
        return;
    }

	if ( cache_changed || ( m_nextUpdate != -1 && ( m_nextUpdate == 0 || timing >= m_nextUpdate ) ) )
	{
        //*m_cachedImage = *image;
        m_cachedImage->fill( 0xff000000 );
        m_nextUpdate = bg->doDraw( m_cachedImage/*, timing - m_eventTiming */);
        //m_cachedimage.save("/tmp/draw2.png", "png");
    }

    //QPainter p( &image );

	// Stretch the image to fit the window.
	// We cannot use Qt::KeepAspectRatioByExpanding as it takes the left top of the image, while we would
	// center it
	//QImage scaled = m_cachedImage.scaled( image.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation );
	//p.drawImage( (image.width() - scaled.width()) / 2, (image.height() - scaled.height()) / 2, scaled );

	// Calculate which way to scale/shrink the image
    double width_ratio = (double) m_cachedImage->width() / (double) image.width();
    double height_ratio = (double) m_cachedImage->height() / (double) image.height();

	// Use the smallest ratio
    double ratio = std::min( width_ratio, height_ratio );

//	qDebug("using ratio %g, %dx%d -> %gx%g", ratio, m_cachedImage.width(), m_cachedImage.height(), m_cachedImage.width() / ratio, m_cachedImage.height() / ratio );

	// Get the scaled image
    Image scaled = m_cachedImage->scaled( m_cachedImage->width() / ratio, m_cachedImage->height() / ratio);//, Qt::KeepAspectRatio, Qt::SmoothTransformation );

	// Draw the part of scaled image
    image.drawImage( scaled,
                (scaled.width() - image.width()) / 2,
                (scaled.height() - image.height()) / 2,
                image.width(), image.height() );
    //m_cachedImage->save("/tmp/bg2.png", "png");

	// Adjust nextUpdate as there may be more events
	if ( m_nextUpdate == -1 )
	{
		found++;

		if ( found != m_preparedEvents.end() )
            m_nextUpdate = found->first - 250; // in advance
	}
}
