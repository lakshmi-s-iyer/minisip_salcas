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


/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef RTCPPACKET_H
#define RTCPPACKET_H

#ifdef _MSC_VER
#ifdef LIBMINISIP_EXPORTS
#define LIBMINISIP_API __declspec(dllexport)
#else
#define LIBMINISIP_API __declspec(dllimport)
#endif
#else
#define LIBMINISIP_API
#endif



#include<vector>
#include<libminisip/RtcpReport.h>

#define RTCP_TYPE_SR		0
#define RTCP_TYPE_RR		1
#define RTCP_TYPE_SDES		2
#define RTCP_TYPE_BYE		3
#define RTCP_TYPE_APP		4

using namespace std;

class LIBMINISIP_API RtcpPacket{
	public:
		RtcpPacket();
		RtcpPacket(void *buildfrom, int length);
		~RtcpPacket();
		vector<RtcpReport *> &get_reports();
		void add_report(RtcpReport *report);

		void debug_print();

		int get_type();
	private:
		vector<RtcpReport *> reports;
		int type;
		
};

#endif
