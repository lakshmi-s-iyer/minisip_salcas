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

#ifndef SESSION_H
#define SESSION_H

#include<libmutil/MemObject.h>
#include"../sip/SipDialogSecurityConfig.h"
#include<libmikey/keyagreement.h>

class MediaStream;
class SdpPacket;
class SdpHeaderM;
//class KeyAgreement;
class IPAddress;

class Session : public MObject{
	public:
		Session( std::string localIp, SipDialogSecurityConfig &config );
		void start();
		void stop();

		MRef<SdpPacket *> getSdpOffer();
		MRef<SdpPacket *> getSdpAnswer();
		
		/* returns whether or not we accept the answer */
		bool setSdpAnswer( MRef<SdpPacket *> answer );
		bool setSdpOffer ( MRef<SdpPacket *> offer );

		void addMediaStreamReceiver( MRef<MediaStream *> );
		void addMediaStreamSender( MRef<MediaStream *> );

		std::string getErrorString();

		bool isSecure();

		virtual std::string getMemObjectType(){return "Session";}


	private:
		/* Key management handling */
		std::string initiatorCreate();
		bool initiatorAuthenticate( std::string message );
		std::string initiatorParse();

		bool responderAuthenticate( std::string message );
		std::string responderParse();

		void addStreamsToKa();

		
		MRef<SdpPacket *> emptySdp();
		MRef<MediaStream *> matchFormat( MRef<SdpHeaderM *> m, uint32_t iFormat, IPAddress * remoteAddress );

		std::list< MRef<MediaStream *> > mediaStreamReceivers;
		std::list< MRef<MediaStream *> > mediaStreamSenders;
		MRef<KeyAgreement *> ka;
		std::string localIpString;
		MRef<SdpPacket *> sdpAnswer;
		bool secured;

		std::string errorString;
		SipDialogSecurityConfig securityConfig;

};

#endif
