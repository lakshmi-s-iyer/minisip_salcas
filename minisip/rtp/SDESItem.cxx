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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include"SDESItem.h"
#include"SDES_CNAME.h"
#include"SDES_NAME.h"
#include"SDES_EMAIL.h"
#include"SDES_PHONE.h"
#include"SDES_LOC.h"
#include"SDES_TOOL.h"
#include"SDES_NOTE.h"

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

using namespace std;


SDESItem *SDESItem::build_from(void * from, int max_length){

	unsigned char* cptr = (unsigned char *)from;

	switch (*cptr){
		case CNAME:
			return new SDES_CNAME(from, max_length);
			break;
		case NAME:
			return new SDES_NAME(from, max_length);
			break;
		case EMAIL:
			return new SDES_EMAIL(from, max_length);
			break;
		case PHONE:
			return new SDES_PHONE(from, max_length);
			break;
		case LOC:
			return new SDES_LOC(from, max_length);
			break;
		case TOOL:
			return new SDES_TOOL(from, max_length);
			break;
		case NOTE:
			return new SDES_NOTE(from, max_length);
			break;
		default:
			;
#ifdef DEBUG_OUTPUT
			cerr <<"ERROR: Parsed SDES item of unknown type ("<<*cptr<<")"<< endl;
#endif
	}
	return NULL;

}

