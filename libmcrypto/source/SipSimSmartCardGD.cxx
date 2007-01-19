/*
  Copyright (C) 2005, 2004 Erik Eliasson, Pan Xuan
  
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
 *          Pan Xuan <xuan@kth.se>
*/

#include <libmcrypto/SipSimSmartCardGD.h>
#include <libmcrypto/SmartCardException.h>
#include <libmcrypto/sha1.h>

using namespace std;

SipSimSmartCardGD::SipSimSmartCardGD():SmartCard() {
	
	this->sendBufferLength = 0;
	this->recvBufferLength = 0;
	this->sendBuffer = NULL;
	this->recvBuffer = NULL;
	this->verifiedCard = 0;
	this->sw_1_2 = 0x0000;
	this->blockedCard = 0;
	this->userAttemptTimer = 3;
	this->adminAttemptTimer = 3;

	if(!selectMikeyApp())
		throw SmartCardException("the MIKEY applet is not usable"); 

}


SipSimSmartCardGD::~SipSimSmartCardGD() {

	clearBuffer();
}

void SipSimSmartCardGD::clearBuffer(){

	if(sendBuffer){
		delete [] sendBuffer;
		sendBuffer=NULL;
	}

	if(recvBuffer){
		delete [] recvBuffer;
		recvBuffer=NULL;
	}
}


bool SipSimSmartCardGD::selectMikeyApp(){
	sendBufferLength = 17;
	recvBufferLength = 2;
	clearBuffer();
	sendBuffer = new unsigned char[sendBufferLength];
	recvBuffer = new unsigned char[recvBufferLength];
	memset(sendBuffer, 0, sendBufferLength);
	memset(recvBuffer, 0, recvBufferLength);
	 
	sendBuffer[0] = 0x00;
	sendBuffer[1] = 0xA4;
	sendBuffer[2] = 0x04;
	sendBuffer[3] = 0x00;
	sendBuffer[4] = 0x0C;
	sendBuffer[5] = 0xD2;
	sendBuffer[6] = 0x76;
	sendBuffer[7] = 0x00;
	sendBuffer[8] = 0x00;
	sendBuffer[9] = 0x05;
	sendBuffer[10] = 0xAC;
	sendBuffer[11] = 0x04;
	sendBuffer[12] = 0x03;
	sendBuffer[13] = 0x11;
	sendBuffer[14] = 0x01;
	sendBuffer[15] = 0x01;
	sendBuffer[16] = 0x01;

	transmitApdu(sendBufferLength, sendBuffer, recvBufferLength, recvBuffer);
	sw_1_2 = recvBuffer[0] << 8 | recvBuffer[1];
	clearBuffer();
	if(sw_1_2 == 0x9000)
		return true;
	else if(sw_1_2 == 0x6999){
		throw SmartCardException("Mikey Applet selection failed");
	}
	else 
		return false;
	
}


bool SipSimSmartCardGD::verifyPin(int verifyMode){

	recvBufferLength = 2; // status word, 16 bits
	
	clearBuffer();
	recvBuffer = new unsigned char[recvBufferLength];
	memset(recvBuffer, 0, recvBufferLength);
	
	if(verifyMode == 0 && (userAttemptTimer > 0) && (userAttemptTimer <= 3)  && blockedCard == 0){

		sendBufferLength = 10; // 1+1+1+1+1+4 (CLA+INS+P1+P1+LEN+DATA)
		sendBuffer = new unsigned char[sendBufferLength];
		memset(sendBuffer, 0, sendBufferLength);

		sendBuffer[0] = 0xB0;
		sendBuffer[1] = 0x10;
		sendBuffer[2] =	0x00;
		sendBuffer[3] = 0x00;
		sendBuffer[4] = 0x04;
		/* the Intel CPU makes use of little-endian format which means the least significant character is stored on the lowest 
		memory address. The communication between PC and SC remains big-endian convention,which means the most significant bit 
		will be transmitted to the peer at first */

		memcpy(&sendBuffer[5], userPinCode, 4);
		sendBuffer[9] = 0x00;
		transmitApdu(sendBufferLength, sendBuffer, recvBufferLength, recvBuffer);
		sw_1_2 = recvBuffer[0] << 8 | recvBuffer[1];
		clearBuffer();
			switch (sw_1_2){
			case 0x9000:
				userAttemptTimer = 3;
				blockedCard = 0;
				verifiedCard = 1;
				return true;
			
			case 0x63C2:
				userAttemptTimer = 2;
				blockedCard = 0;
				break;
			
			case 0x63C1:
				userAttemptTimer = 1;
				blockedCard = 0;
				break;
			
			case 0x63C0:
				userAttemptTimer = 0;
				blockedCard = 1;
				break;

			case  0x6A88:
				throw SmartCardException("Pin not found. Wrong P2 value");
				
			default:
				throw SmartCardException("Unknown return value of SW1 SW2");

			}
			return false;
	}

	else if(verifyMode == 1 && (adminAttemptTimer > 0 && adminAttemptTimer <= 3) && (blockedCard == 0 || blockedCard == 1)){
			sendBufferLength = 14;
			sendBuffer = new unsigned char[sendBufferLength];
			memset(sendBuffer, 0, sendBufferLength);
			sendBuffer[0] = 0xB0;
			sendBuffer[1] = 0x10;
			sendBuffer[2] =	0x00;
			sendBuffer[3] = 0x01;
			sendBuffer[4] = 0x08;
			/* the Intel CPU makes use of little-endian format which means the least significant character is stored on the lowest 
			memory address. The communication between PC and SC remains big-endian convention,which means the most significant bit 
			will be transmitted to the peer at first */

			memcpy(&sendBuffer[5], adminPinCode, 8);
			sendBuffer[13] = 0x00;
			
			transmitApdu(sendBufferLength, sendBuffer, recvBufferLength, recvBuffer);
			sw_1_2 = recvBuffer[0] << 8 | recvBuffer[1];

			clearBuffer();
						
			switch (sw_1_2){
			case 0x9000:
				adminAttemptTimer = 3;
				userAttemptTimer = 3;
				blockedCard = 0;								// to validate the userPinCode by a right adminPinCode
				verifiedCard = 2;
				return true;

			case 0x63C2:
				adminAttemptTimer = 2;
				blockedCard = 1;
				break;

			case 0x63C1:
				adminAttemptTimer = 1;
				blockedCard = 1;
				break;

			case 0x63C0:
				adminAttemptTimer = 0;
				blockedCard = 2;
				break;

			case  0x6A88:
				throw SmartCardException("Pin not found. Wrong P2 value");
				
			default:
				throw SmartCardException("Unknown return value of SW1 SW2");
			}
			return false;
	}
	else
		throw SmartCardException("Unknown authentication mode. The card is unauthorized.");

}

bool SipSimSmartCardGD::changePin(const char * newPinCode){
	
	if(establishedConnection == true && verifiedCard == 2){
		setPin(newPinCode);
		sendBufferLength = 10;
		recvBufferLength = 2;
		clearBuffer();
		sendBuffer = new unsigned char[sendBufferLength];
		recvBuffer = new unsigned char[recvBufferLength];
		memset(sendBuffer, 0, sendBufferLength);
		memset(recvBuffer, 0, recvBufferLength);

		sendBuffer[0] = 0xB0;
		sendBuffer[1] = 0xD0;
		sendBuffer[2] = 0x00;
		sendBuffer[3] = 0x00;
		sendBuffer[4] = 0x04;
		memcpy(&sendBuffer[5], userPinCode, 4);
		sendBuffer[9] = 0x00;
		
		transmitApdu(sendBufferLength, sendBuffer, recvBufferLength, recvBuffer);
		sw_1_2 = recvBuffer[0] << 8 | recvBuffer[1];
		clearBuffer();
		switch(sw_1_2){
			case 0x9000:
				break;
			case 0x6982:
				throw SmartCardException("authentication level is not sufficient");
			case 0x6A88:
				throw SmartCardException("PIN not found");
			default:
				throw SmartCardException("Unknown status code was returned");
		}
		return true;
	}
	else
		throw SmartCardException("Either the smart card connection has not been established or access level is not sufficient");
}

bool SipSimSmartCardGD::getRandomValue(unsigned char * randomPtr, unsigned long randomLength){
	if(establishedConnection == true && verifiedCard == 1 && blockedCard == 0){

		unsigned char * tempBuffer;
		sendBufferLength = 5;
		recvBufferLength = 2 + randomLength;

		clearBuffer();
		sendBuffer = new unsigned char[sendBufferLength];
		recvBuffer = new unsigned char[recvBufferLength];
		memset(sendBuffer, 0, sendBufferLength);
		memset(recvBuffer, 0, recvBufferLength);
		tempBuffer = (unsigned char *) &randomLength;

		sendBuffer[0] = 0xB0;
		sendBuffer[1] = 0x40;
		sendBuffer[2] = 0x00;
		sendBuffer[3] = 0x00;
		sendBuffer[4] = *tempBuffer;

		transmitApdu(sendBufferLength, sendBuffer, recvBufferLength, recvBuffer);
		
		sw_1_2 = recvBuffer[randomLength] << 8 | recvBuffer[randomLength + 1];
		switch(sw_1_2){
			case 0x9000:
				break;
			case 0x6008:
				clearBuffer();
				return false;
				//throw SmartCardException("failed to generate random value from G&D smart card");
			default:
				clearBuffer();
				return false;
				//throw SmartCardException("Unknown state value was returned when generating random value");
		}
		
		memcpy(randomPtr, recvBuffer,randomLength);
		
		clearBuffer();
		return true;
	}
	else
		throw SmartCardException("unconnected card or the user doesn't have proper access level. Correct userPinCode is required");
}

bool SipSimSmartCardGD::getSignature(unsigned char *dataPtr, int dataLength, unsigned char *signaturePtr, int & signatureLength, 
									 bool doHash, int hash_alg)
{
	if(establishedConnection == true && verifiedCard == 1 && blockedCard ==0){	
		unsigned char * messageDigestPtr;
		unsigned long messageDigestLength;
		if (doHash){ 
			assert( hash_alg == HASH_SHA1 );
			messageDigestPtr = new unsigned char[20];
			sha1(dataPtr, dataLength, messageDigestPtr);
			messageDigestLength=20;
		}else{
			messageDigestPtr = dataPtr;
			messageDigestLength=dataLength;
		}
		
		sendBufferLength = 6+messageDigestLength;
		recvBufferLength = 130;											// this time we don't know the size of the receive buffer. Assume 128 is big enough and we
																		// send the reference of recvBufferLength to this function and get that actual size from it 
		clearBuffer();
		sendBuffer = new unsigned char[sendBufferLength];
		recvBuffer = new unsigned char[recvBufferLength];
		memset(sendBuffer, 0, sendBufferLength);
		memset(recvBuffer, 0, recvBufferLength);
		
		sendBuffer[0] = 0xB0;
		sendBuffer[1] = 0x42;
		sendBuffer[2] = 0x10;
		sendBuffer[3] = 0x00;
		sendBuffer[4] = 0x14;				// sha-1 has 20 bytes (160 bits) output as message digest
		memcpy(&sendBuffer[5], messageDigestPtr, messageDigestLength);
		sendBuffer[5+messageDigestLength] = 0x80;

		transmitApdu(sendBufferLength, sendBuffer, recvBufferLength, recvBuffer);
		
		sw_1_2 = recvBuffer[recvBufferLength - 2] << 8 | recvBuffer[recvBufferLength - 1];
			switch(sw_1_2){
				case 0x9000:
					break;
				case 0x6004:
					clearBuffer();
					return false;
					//throw SmartCardException("failed to sign the message digest on the smart card");
				default:
					clearBuffer();
					return false;
		 			//throw SmartCardException("Unknown state value was returned when signing the message digest");
		}
		signatureLength = recvBufferLength - 2;
		memcpy(signaturePtr, recvBuffer, signatureLength);
		clearBuffer();
		if (messageDigestPtr)
			delete [] messageDigestPtr;
		
		return true;
	}
	else
		throw SmartCardException("unconnected card or the user doesn't have proper access level. Correct userPinCode is required");
}


bool SipSimSmartCardGD::getTek(unsigned char csId, unsigned long csbIdValue,
			       unsigned char * tgkPtr, unsigned long tgkLength,
			       unsigned char * tekPtr, unsigned long tekLength){

	if(establishedConnection == true && verifiedCard == 1 && blockedCard == 0){
		
		unsigned char * tempLength;
		tempLength = (unsigned char *) & tgkLength;
		
		sendBufferLength = tgkLength + 11;
		recvBufferLength = 255;
		
		clearBuffer();
		sendBuffer = new unsigned char[sendBufferLength];
		recvBuffer = new unsigned char[recvBufferLength];
		memset(sendBuffer, 0, sendBufferLength);
		memset(recvBuffer, 0, recvBufferLength);

		sendBuffer[0] = 0xB0;
		sendBuffer[1] = 0x44;
		sendBuffer[2] = 0x00;
		sendBuffer[3] = 0x00;
		sendBuffer[4] = tempLength[0];
		sendBuffer[5] = csId;
		sendBuffer[6] = (unsigned char)((csbIdValue) >> 24 & 0xFF);
		sendBuffer[7] = (unsigned char)((csbIdValue) >> 16 & 0xFF);
		sendBuffer[8] = (unsigned char)((csbIdValue) >> 8 & 0xFF);
		sendBuffer[9] = (unsigned char)(csbIdValue & 0xFF);
		memcpy(&sendBuffer[10], tgkPtr, tgkLength);
		sendBuffer[tgkLength + 10] = 0xFF;
		
		transmitApdu(sendBufferLength, sendBuffer, recvBufferLength, recvBuffer);
		sw_1_2 = recvBuffer[recvBufferLength - 2] << 8 | recvBuffer[recvBufferLength - 1];
		switch(sw_1_2){
			case 0x9000:
				break;
			case 0x6007:
				clearBuffer();
				throw SmartCardException("failed to get the Diffie-Hellman public key from the smart card");
			default:
				clearBuffer();
				throw SmartCardException("Unknown state value was returned when getting the Diffie-Hellman public key from the smart card");
		}
	//	tekLength = recvBufferLength - 2;
		memcpy(tekPtr, recvBuffer, tekLength);
		clearBuffer();
		return true;
	}
	else
		throw SmartCardException("unconnected card or unauthorized card");

}


/*bool SipSimSmartCardGD::getTekPk(unsigned char csId, unsigned long csbIdValue,
				    unsigned long tgkLength, unsigned char * tgkPtr,
                                   unsigned long & tekLength, unsigned char * tekPtr){
	return true;
}*/

bool SipSimSmartCardGD::getDHPublicValue(unsigned long & dhPublicValueLength, unsigned char * dhPublickValuePtr){

	if(establishedConnection == true && verifiedCard == 1 && blockedCard == 0){	
		sendBufferLength = 	5;
		recvBufferLength = 255;

		clearBuffer();
		sendBuffer = new unsigned char[sendBufferLength];
		recvBuffer = new unsigned char[recvBufferLength];
		memset(sendBuffer, 0, sendBufferLength);
		memset(recvBuffer, 0, recvBufferLength);
		
		sendBuffer[0] = 0xB0;
		sendBuffer[1] = 0x20;
		sendBuffer[2] = 0x00;
		sendBuffer[3] = 0x00;
		sendBuffer[4] = 0xFF;
			
		transmitApdu(sendBufferLength, sendBuffer, recvBufferLength, recvBuffer);
		sw_1_2 = recvBuffer[recvBufferLength - 2] << 8 | recvBuffer[recvBufferLength - 1];
		switch(sw_1_2){
			case 0x9000:
				break;
			case 0x6001:
				clearBuffer();
				throw SmartCardException("failed to get the TEK from the smart card");
			default:
				clearBuffer();
				throw SmartCardException("Unknown state value was returned when generating TEK from the smart card");
		}
		
		dhPublicValueLength = recvBufferLength - 2;
		memcpy(dhPublickValuePtr, recvBuffer, dhPublicValueLength);
		clearBuffer();
		return true;
	}
	else
		throw SmartCardException("unconnected card or the user doesn't have proper access level. Correct userPinCode is required");
}

bool SipSimSmartCardGD::generateKeyPair(){
	
	if(establishedConnection == true && verifiedCard == 2 && blockedCard == 0){	
		
		sendBufferLength = 4;
		recvBufferLength = 2;
		clearBuffer();
		sendBuffer = new unsigned char[4];
		recvBuffer = new unsigned char[2];
		memset(sendBuffer, 0, 4);
		memset(recvBuffer, 0, 2);
		
		sendBuffer[0] = 0xB0;
		sendBuffer[1] = 0xD3;
		sendBuffer[2] = 0x00;
		sendBuffer[3] = 0x00;
		
		transmitApdu(sendBufferLength, sendBuffer, recvBufferLength, recvBuffer);
	 	
		sw_1_2 = recvBuffer[0] << 8 | recvBuffer[1];
		clearBuffer();
		
		switch(sw_1_2){
			case 0x9000:
				return true;
			case 0x6982:
				throw SmartCardException("authorization level insufficient");
			default:
				throw SmartCardException("Unknown state value was returned when generating the key pair on the smart card");
		}
	}
	else
		throw SmartCardException("unconnected card or the user doesn't have proper access level. Correct userPinCode is required");

}

bool SipSimSmartCardGD::getPublicKey(unsigned long publicKeyLength, unsigned char * publicKeyPtr, int keyPairType){

	//if(establishedConnection == true && verifiedCard == 1 && blockedCard == 0){	
	//	sendBufferLength = 4;
	//	
	//	switch(keyPairType){
	//		case 0:
	//			recvBufferLength = 154;
	//		case 1:
	//			recvBufferLength = 26;
	//		default:
	//			throw SmartCardException(Unknown key pair type value);
	//	}

	//}
	//else
	//	throw SmartCardException("unconnected card or the user doesn't have proper access level. Correct userPinCode is required");
		

}
