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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include "XvDisplay.h"
#include<sys/time.h>
#include"../VideoException.h"

using namespace std;

#define NB_IMAGES 3

XvDisplay::XvDisplay( uint32_t width, uint32_t height):X11Display( width, height){
	xvPort = -1;
}

void XvDisplay::openDisplay(){
	char * displayName = NULL;
	unsigned int i,j,k;
	unsigned int nAdaptors;
	XvAdaptorInfo * adaptors;

	xvPort = -1;
	

	displayName = getenv( "DISPLAY" );

	if( displayName == NULL ){
		throw VideoException( "Could not open X11 display" );
	}
	
	/* Open the display */

	display = XOpenDisplay( displayName );

	if( display == NULL ){
		throw VideoException( "Could not open X11 display" );
	}

	screen = DefaultScreen( display );

	/* Query the Xv extension */

	if( XvQueryExtension( display, &i, &i, &i, &i, &i ) != Success ){
		throw VideoException( "Could not find the Xv extension" );
	}

	/* Query the Xv adaptors */

	if( XvQueryAdaptors( display, DefaultRootWindow( display ),
			     &nAdaptors, &adaptors ) != Success ){
		throw VideoException( "Could not find Xv adaptors" );
	}

	for( i = 0; i < nAdaptors; i++ ){

		
		XvImageFormatValues * formats;
		int nFormats;
		int j;
			
		if( !( adaptors[i].type & XvInputMask ) | 
		    !(adaptors[i].type & XvImageMask ) ){
			continue;
		}

		formats = XvListImageFormats( display, 
				              adaptors[ i ].base_id,
					      &nFormats );
	

		for( j = 0; j < nFormats; j++ ){
			if( formats[ j ].id != M_CHROMA_I420 ){
				continue;
			}

			for( k = adaptors[i].base_id; 
			     k < adaptors[i].base_id + adaptors[i].num_ports;
			     k ++ ){
				if( XvGrabPort( display, k, CurrentTime )
					== Success ){
					xvPort = k;
					break;
				}
			}

			if( xvPort != -1 ){
				break;
			}
		}

		if( xvPort != -1 ){
			break;
		}

	}

	if( xvPort == -1 ){
		throw VideoException( "Could not find a suitable Xv Port" );
	}
}

void XvDisplay::init( uint32_t width, uint32_t height ){
	this->width = width;
	this->height = height;
}

void XvDisplay::destroyWindow(){
	XvUngrabPort( display, xvPort, CurrentTime );
	XSync( display, False );
	fprintf( stderr, "Destroying video window\n");
	XDestroyWindow( display, videoWindow );
	XFreeGC( display, gc );

	fprintf( stderr, "Destroying base window\n");
	XUnmapWindow( display, baseWindow );
	XDestroyWindow( display, baseWindow );
	XCloseDisplay( display );
}

MImage * XvDisplay::allocateImage(){
	

	char * imageData = ( char * )malloc( width * height * 3 );
	XvImage * image= XvCreateImage( display, xvPort, M_CHROMA_I420,
			       imageData, width, height );

	
	MImage * mimage;

	mimage = new MImage;

	for( unsigned int i = 0; i < 3; i++ ){
		mimage->data[i] = (uint8_t *)(image->data + image->offsets[i]);
		mimage->linesize[i] = image->pitches[i];
	}

	mimage->privateData = image;

	return mimage;
	
}

void XvDisplay::deallocateImage( MImage * mimage ){
	XvImage * image = (XvImage *)mimage->privateData;

	XFree( image );
	
	delete mimage;
	
}

bool XvDisplay::handlesChroma( uint32_t chroma ){
	return chroma == M_CHROMA_I420;
}


void XvDisplay::displayImage( MImage * mimage ){

	XvPutImage( display, xvPort, videoWindow, gc,
                    (XvImage*)(mimage->privateData),
                    0 /*src_x*/, 0 /*src_y*/,
                    width, height,
                    0 /*dest_x*/, 0 /*dest_y*/, baseWindowWidth, baseWindowHeight );
}

uint32_t XvDisplay::getRequiredWidth(){
	return width;
}

uint32_t XvDisplay::getRequiredHeight(){
	return height;
}