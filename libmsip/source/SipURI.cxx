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

/* Name
 * 	SipURI.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/

#include<config.h>

#include<libmsip/SipURI.h>
#include<libmsip/SipException.h>
#include<libmutil/itoa.h>
#include<libmutil/trim.h>
#include<libmutil/dbg.h>

#ifdef DEBUG_OUTPUT
#include<iostream>
#endif

//TODO: Fix better parser. handle for example: 1234567;user=phone
SipURI::SipURI(string build_from)/*:MObject("SipURI")*/{

	this->port=0;
	this->type="";
	this->user_id="";
	this->transport="";
	ip="";
	string port="";
	user_id="";
	
	unsigned i=4;
	string part1;
	while (build_from[i]==' ')
		i++;

	while (!(build_from[i]==' ' || build_from[i]==':' || build_from[i]=='@' || build_from[i]==';')){
		if (i == build_from.size())
			throw new SipExceptionInvalidURI();
		part1+=build_from[i];
		i++;
	}
        
	if (build_from[i]=='@'){
		i++;
		user_id=part1;
		while (!(build_from[i]==':' || build_from[i]==' ' || build_from[i]==';' || i>=build_from.length())){
			ip+=build_from[i];
			i++;
		}
		if ((i != build_from.length())&& (build_from[i]==':')){
			i++;
			while (!(build_from[i]==';' || i>=(build_from.length()))){
				port+=build_from[i];
				i++;
			}
			this->port = atoi(port.c_str());
		}
	}else{
		ip=part1;
		if (build_from[i]==':'){
			i++;
			while (!(build_from[i]==' ' || build_from[i]==';' ||  i>=build_from.length())){
				port+=build_from[i];
				i++;
			}
			this->port = atoi(port.c_str());
			
		}else{
//			merr << "Somethihng else happened - CHECK"<< end;
			if (build_from[i]==' ')
				ip=part1;
			else
				throw new SipExceptionInvalidURI();
				//merr << "ERROR while parsing SIP URI"<< end;
		}
		
	}

	i=0; 
	for (int32_t j=build_from.length()-2; j>0; j--){
		if (build_from[j]==';')
			i=j;
	}
	
	unsigned len=build_from.length();
	if (build_from.substr(i,6) == ";user="){
		i+=6;
		while (!(build_from[i]=='>' || 
                            build_from[i]==';' || 
                            build_from[i]==' ' || 
                            i>= len)){
			type+=build_from[i];
			i++;
		}
	}
        
	if (port.length()>0)
		this->port=atoi(port.c_str());

//        merr << "SipUri constructor:After parsing:\n\tusers_name=<"<<users_name<<
//                ">\n\tuser_id=<"<<user_id<<
//                ">\n\tip=<"<<ip<<
//                ">\n\tport=<"<<port<<
//                ">\n\ttype=<"<<type<<
//                ">\n\ttransport=<"<<transport<<">"<<end;

}

SipURI::SipURI(string id, string ip, string type,int32_t port)/*:MObject("SipURI")*/{
	this->user_id=id;
	this->ip=ip;
	this->port=port;
	this->type=type;
	this->users_name="";
//	this->tag="";
	this->transport="";
}


void SipURI::setUserId(string id){
	user_id=id;
}

string SipURI::getUserId(){
	return user_id;
}

void SipURI::setIp(string ip){
	this->ip=ip;
}

string SipURI::getIp(){
	return ip;
}

void SipURI::setPort(int32_t port){
	this->port=port;
}

int32_t SipURI::getPort(){
	return port;
}

void SipURI::setUserType(string type){
	this->type=type;
}

string SipURI::getUserType(){
	return type;
}

void SipURI::setTransport(string transp){
	transport = transp;
}

string SipURI::getTransport(){
	return transport;
}

string SipURI::getString(){
//	merr << "SipURI::getString"<< end;
	string uri="sip:";
//	merr << "uri so far: "<< uri<<end;
	if (user_id.length()>4 && user_id.substr(0,4)=="sip:")
		uri="";
	
//	merr << "uri so far(2): "<< uri<<end;
	if (user_id.length()>0)
		uri+=user_id;
//	merr << "uri so far(3): "<< uri<<end;
	if (user_id.find("@")==string::npos){		
		uri=uri+"@"+ip;
	}
//	merr << "uri so far(4): "<< uri<<end;
	if (port!=0)
		uri=uri+":"+itoa(port);
//	merr << "uri so far(5): "<< uri<<end;
	if (type!="")
		uri=uri+";user="+type;
//	merr << "uri so far(6): "<< uri<<end;
//	if (tag!="")
//		uri=uri+";tag="+tag;
	if (transport!="")
		uri=uri+";transport="+transport;
//	merr << "uri so far(7) (final): "<< uri<<end;
	return uri;
}

string SipURI::getUserIpString(){
	string ret="sip:";
	if (user_id.length()>4 && user_id.substr(0,4)=="sip:")
		ret="";
	
	if (user_id.length()>0)
		ret=ret+user_id;
	if (user_id.length()>0 &&user_id.find("@")==string::npos){
		ret=ret+"@"+ip;
	}
	return ret;
}

void SipURI::setUsersName(string name){
	this->users_name=name;
}

string SipURI::getUsersName(){
	return users_name;
}
