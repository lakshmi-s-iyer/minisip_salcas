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
 * 	SipTransactionServer.h
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se
 * Purpose
 * 
*/



#ifndef SIPTRANSACTIONSERVER_H
#define SIPTRANSACTIONSERVER_H

#include<libmsip/SipSMCommand.h>
#include<libmsip/SipTransaction.h>
#include<libmsip/SipInvite.h>
#include<libmsip/SipResponse.h>

class SipTransactionServer: public SipTransaction{
	public:
		SipTransactionServer(MRef<SipDialog*> d, int seq_no, const string &branch, string callid);
		
		virtual ~SipTransactionServer();

		virtual string getName(){return "transaction_server[branch="+getBranch()+",type="+getDebugTransType()+"]";}

		virtual bool handleCommand(const SipSMCommand &command);

		void setUpStateMachine();

	private:

		bool a0_start_trying_request(const SipSMCommand &command);
		bool a1_trying_proceeding_1xx(const SipSMCommand &command);
		bool a2_trying_completed_non1xxresp(const SipSMCommand &command);
		bool a3_proceeding_completed_non1xxresp(const SipSMCommand &command);
		bool a4_proceeding_proceeding_request(const SipSMCommand &command);
		bool a5_proceeding_proceeding_1xx(const SipSMCommand &command);
		bool a6_proceeding_terminated_transperr(const SipSMCommand &command);
		bool a7_completed_completed_request(const SipSMCommand &command);
		bool a8_completed_terminated_transperr(const SipSMCommand &command);
		bool a9_completed_terminated_timerJ(const SipSMCommand &command);

//		MRef<SipMessage*> received_request;
//		string call_id;
		MRef<SipResponse*> lastResponse;

		int timerT1, timerJ;
		
		int32_t command_seq_no;
};

#endif
