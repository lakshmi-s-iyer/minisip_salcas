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


#ifndef EERUNNABLE_H
#define EERUNNABLE_H

#include<string>

using namespace std;

#ifdef _MSC_VER
#ifdef LIBMUTIL_EXPORTS
#define LIBMUTIL_API __declspec(dllexport)
#else
#define LIBMUTIL_API __declspec(dllimport)
#endif
#else
#define LIBMUTIL_API
#endif

class LIBMUTIL_API ThreadException{
public:
	ThreadException(std::string description);
	std::string what();
private:
	std::string desc;
};

class LIBMUTIL_API Runnable{
public:
	virtual void run()=0;
};


class LIBMUTIL_API Thread{
public:
	Thread(Runnable *runnable);
	static int createThread( void f());
	static int createThread( void* f(void*), void *arg);
private:
	void *handle_ptr;
};

#endif
