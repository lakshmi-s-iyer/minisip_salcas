/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/



#include<config.h>

#if defined WIN32 || defined _MSC_VER
#include <sys/timeb.h>
#else
#include<sys/time.h>
#include<time.h>
#endif

uint64_t mtime(){
#if defined WIN32 || defined _MSC_VER
	struct _timeb tb;
	_ftime (&tb);

	return ((uint64_t)tb.time) * 1000LL + ((uint64_t)tb.millitm);
#else // FIXME: do a proper check for gettimeofday
	struct timeval tv;

	gettimeofday( &tv, NULL );

	return ((uint64_t)tv.tv_sec) * (uint64_t)1000 + ((uint64_t)tv.tv_usec) / (uint64_t)1000;
#endif
}
