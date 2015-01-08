//
// parser.c
//
//  Created on: Dec 6, 2014
//      Author: Bill
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "parser.h"

#include "../src/timeLoop.h"
#include "../utilities/nwInterface.h"

#ifndef	DISABLE_IO
extern	mraa_gpio_context gpio;
extern	mraa_aio_context	vDet;
extern	mraa_gpio_context	iSense;
#endif	// DISABLE_IO

extern int		running;


//--	----	----	----


char *getADCData( void ) {

#ifdef	DISABLE_IO
	return NULL;
#else	// DISABLE_IO
	togglePin( gpio );
	return NULL;
#endif	// DISABLE_IO
}


//--	----	----	----


void putCommand( char type, char value ) {

	putc( DC_SEND_HEADER, stdout);
	putc( type, stdout);
	putc( value, stdout);
	usleep( 20000 );

/*
	printf( "Output type: 0x%02X, value: 0x%02X\n", type & 0x0FF, value & 0x0FF );
*/
}

//--	----	----	----


char *parseCommand( char *command ) {

	if ( 0 == strcmp( "toggle", command ) ) {
//		printf( "\n  Got toggle !!\n" );
#ifdef	DISABLE_IO
		return NULL;
#else	// DISABLE_IO
		togglePin( gpio );
		return NULL;
#endif	// DISABLE_IO
	}

	if ( 0 == strcmp( "exit", command ) ) {			// If command to terminate this program is entered
//		printf( "\n\n  Got exit !!\n\n" );
///		pthread_exit( NULL );						// Kill thread
///		exit( 1 );									// Kill program
		running = 1;
		return NULL;
	}

	if ( 0 == strcmp( "adcData1", command ) ) {	// null command if empty uri entered
//		printf( "  Got adcData1\n" );
		return getADCData();
	}

	char *buffer = malloc( 1000 );
	if ( 0 == strcmp( "vDet", command ) ) {	// null command if empty uri entered
//		printf( "  Got vDet, read ADC for vDet on pin 1\n" );
		int adc1 = readAIO( vDet );
		sprintf( buffer, "%d", adc1 & 0x03FF );
		return buffer;
	}

	if ( 0 == strcmp( "iSense", command ) ) {	// null command if empty uri entered
//		printf( "  Got iSense, detect signal for iSense on pin 9\n" );
		int pin9 = inputPin( iSense );
		sprintf( buffer, "%d", pin9 );
		return buffer;
	}

	int sz = 0; // = sprintf( buffer, html_head );		// Start with html and head /head tags and opening body tag
	if ( 0 == strcmp( "sample1", command ) ) {

//		printf( "  %s\n", command );
		// Here we sprintf the html contents for display
		sz += sprintf( &buffer[sz], "Data from web parser" );

//		sz += sprintf( &buffer[sz], html_foot );	// String with ending /body and /html tags, finalize the page
		return buffer;
	} else if ( 0 == strcmp( "tDirA", command ) ) {
		putCommand( DC_CMD_DIRA, FW);
	} else if ( 0 == strcmp( "tDirB", command ) ) {
		putCommand( DC_CMD_DIRB, FW);
	} else if ( 0 == strcmp( "hiA", command ) ) {
		putCommand( DC_CMD_PWMA, 240);
	} else if ( 0 == strcmp( "hiB", command ) ) {
		putCommand( DC_CMD_PWMB, 240);
	} else if ( 0 == strcmp( "loA", command ) ) {
		putCommand( DC_CMD_PWMA, 20);
	} else if ( 0 == strcmp( "loB", command ) ) {
		putCommand( DC_CMD_PWMB, 20);
	} else {
		return NULL;
	}
	sz += sprintf( &buffer[sz], command );
	return buffer;
}


// End of parser.c
