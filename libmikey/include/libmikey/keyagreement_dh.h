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

#ifndef KEYAGREEMENT_DH_H
#define KEYAGREEMENT_DH_H

#include<openssl/dh.h>
#include<libmikey/keyagreement.h>
#include<libmikey/oakley_groups.h>

#define DH_GROUP_OAKLEY5 0
#define DH_GROUP_OAKLEY1 1
#define DH_GROUP_OAKLEY2 2


//using namespace std;

class certificate_chain;
class certificate;
class ca_db;

class KeyAgreementDH : public KeyAgreement{
	public:
		KeyAgreementDH( MRef<certificate_chain *> cert, 
				MRef<ca_db *> ca_db );
		KeyAgreementDH( MRef<certificate_chain *> cert, 
				MRef<ca_db *> ca_db, int group );
		~KeyAgreementDH();

		int computeTgk();
		int setGroup( int group );
		int group();
		
		void setPeerKey( byte_t * peerKey, int peerKeyLength );
		int peerKeyLength();
		byte_t * peerKey();
		
		int publicKeyLength();
		byte_t * publicKey();
		
		MRef<certificate_chain *> certificateChain();
		MRef<certificate_chain *> peerCertificateChain();
		void addPeerCertificate( MRef<certificate *> cert );
		int controlPeerCertificate();
	private:
		int groupValue;
		DH * opensslDhPtr;
		byte_t * peerKeyPtr;
		int peerKeyLengthValue;
		MRef<certificate_chain *> certChainPtr;
		MRef<certificate_chain *> peerCertChainPtr;
		MRef<ca_db *> certDbPtr;
};

#endif
