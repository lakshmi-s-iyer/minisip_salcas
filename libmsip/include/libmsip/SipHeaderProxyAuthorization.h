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


/* Name
 * 	SipHeaderProxyAuthorization.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#ifndef SIPHEADERPROXYAUTHORIZATION_H
#define SIPHEADERPROXYAUTHORIZATION_H

#include<libmsip/libmsip_config.h>

#include<libmsip/SipHeaderAuthorization.h>
#include<libmsip/SipUri.h>

/**
 * @author Erik Eliasson
*/

extern SipHeaderFactoryFuncPtr sipHeaderProxyAuthorizationFactory;

class LIBMSIP_API SipHeaderValueProxyAuthorization: public SipHeaderValueAuthorization{
	public:
		SipHeaderValueProxyAuthorization(const std::string &build_from);
		
		SipHeaderValueProxyAuthorization(const std::string &sip_method,
				const std::string &username, 
				const std::string &realm, 
				const std::string &nonce, 
				const std::string &opaque,
				const SipUri &uri, 
				const std::string &auth_id, 
				const std::string &password,
				const std::string &auth_method="DIGEST");
		

		virtual ~SipHeaderValueProxyAuthorization();

                virtual std::string getMemObjectType(){return "SipHeaderProxyAuthorization";}
		
		/**
		 * returns string representation of the header
		 */
		virtual std::string getString(); 

	protected:
		char getFirstParameterSeparator(){return ' ';}
		char getParameterSeparator(){return ',';}

	private:
	
};

#endif
