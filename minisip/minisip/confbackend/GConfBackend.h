/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef G_CONF_BACKEND_H
#define G_CONF_BACKEND_H

#include"ConfBackend.h"

typedef struct _GConfClient GConfClient;

class GConfBackend : public ConfBackend {
	public:
		virtual void save( std::string key, std::string value );
		virtual void save( std::string key, int32_t value );

		virtual std::string loadString( std::string key, std::string defaultValue="" );
		virtual int32_t loadInt( std::string key, int32_t defaultValue=0 );

		virtual void commit();

		~GConfBackend();
		GConfBackend();

	private:
		GConfClient * client;

		void sanitizeKey( string &key );

};

#endif