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


#ifndef _DBG_H
#define _DBG_H

#include<string>

#define DBG_INFO 0
#define DBG_ERROR 1

#ifdef _MSC_VER
#ifdef LIBMUTIL_EXPORTS
#define LIBMUTIL_API __declspec(dllexport)
#else
#define LIBMUTIL_API __declspec(dllimport)
#endif
#else
#define LIBMUTIL_API
#endif

class LIBMUTIL_API DbgEndl{
    public:
        DbgEndl(){}
    private:
        int i;
};


class LIBMUTIL_API Dbg{
    public:
        static const DbgEndl endl;
	Dbg(bool error_output=false, bool enabled=true);
        Dbg &operator<<(std::string);
        Dbg &operator<<(int);
        Dbg &operator<<(char);
        Dbg &operator<<(DbgEndl &endl);
//        void operator<<(bool);
	void setEnabled(bool enabled);
	bool getEnabled();
    private:
	bool error_out;
	bool enabled;
	std::string str;
};

extern LIBMUTIL_API Dbg mout;
extern LIBMUTIL_API Dbg merr;
extern LIBMUTIL_API Dbg mdbg;
extern LIBMUTIL_API DbgEndl end;


#endif

