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

#include"DirectSoundDevice.h"

using namespace std;

DirectSoundDevice::DirectSoundDevice( string device ):SoundDevice( device ){
	nChannelsPlay = 2;
	nChannelsRecord = 2;

	samplingRate = 8000;

	sampleSize = 2;


	
}

DirectSoundDevice::~DirectSoundDevice(){
}

int DirectSoundDevice::openRecord( int samplingRate, int nChannels, int format ){

	return 0;

}

int DirectSoundDevice::openPlayback( int samplingRate, int nChannels, int format ){

	return 0;

}

int DirectSoundDevice::closeRecord(){
	return 0;
}

int DirectSoundDevice::closePlayback(){
	return 0;
}

int DirectSoundDevice::read( byte_t * buffer, uint32_t nSamples ){
	
	return nSamples;
}

int DirectSoundDevice::write( byte_t * buffer, uint32_t nSamples ){


	return nSamples;
}
	
void DirectSoundDevice::sync(){
}
