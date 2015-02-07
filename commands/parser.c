//
// parser.c
//
//  Created on: Dec 6, 2014
//      Author: Bill
//


#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions

#include <termios.h>    // POSIX terminal control definitions

#include "parser.h"

#include "../src/timeLoop.h"
#include "../utilities/nwInterface.h"

#ifndef	DISABLE_IO
extern	mraa_gpio_context 	gpio;
extern	mraa_aio_context	vDet;
extern	mraa_gpio_context	iSense;
#endif	// DISABLE_IO

extern int		running;
extern int		showMonitor;

extern int		userLoopExitCode;
extern int		timeLoopExitCode;
extern int		servLoopExitCode;;
extern int		webLoopExitCode;



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

//    serialWrite( DC_SEND_HEADER );
//    serialWrite( type );
//    serialWrite( value );
	usleep( 20000 );		// ? needed ?


//	printf( "Output type: 0x%02X, value: 0x%02X\n", type & 0x0FF, value & 0x0FF );
}

//--	----	----	----


char *parseUserCommand( char *command ) {

//	printf( "  parseCommand %s\n", command );

	// mostly user commands
	if ( 0 == strcmp( "toggle", command ) ) {
		printf( "\n  Got toggle !!\n" );
#ifdef	DISABLE_IO
		return NULL;
#else	// DISABLE_IO
//		togglePin( gpio );
		return NULL;
#endif	// DISABLE_IO
	}

	if ( 0 == strcmp( "exit", command ) ) {			// If command to terminate this program is entered
//		printf( "\n\n  Got exit !!\n\n" );
///		pthread_exit( NULL );						// Kill thread
///		exit( 1 );									// Kill program
		running = 0;
		return NULL;
	}

	if ( 0 == strcmp( "status1", command ) ) {			// Status Test command
		printf( "  Got status1\n" );

		char *statusString = malloc( 256 );
		sprintf( statusString, "u: %d, t: %d, s: %d, w: %d", userLoopExitCode, timeLoopExitCode, servLoopExitCode, webLoopExitCode );

		return statusString;
	}

	if ( 0 == strcmp( "mOff", command ) ) {			// Serial Test command
//		printf( "  Got mOff\n" );
		showMonitor = 0;
		return NULL;
	}

	if ( 0 == strcmp( "mOn", command ) ) {
//		printf( "  Got mOn\n" );
		showMonitor = 1;
		return NULL;
	}

	if ( 0 == strcmp( "st1", command ) ) {			// Serial Test command
//		printf( "  Got st1\n" );
		setupSerial1( B9600 );
		testRW( "Frog" );
		closeSerial1();
		return NULL;
	}

	if ( 0 == strcmp( "adcData1", command ) ) {
//		printf( "  Got adcData1\n" );
		return getADCData();
	}
	return NULL;
}

//--	----	----	----


char *parseWebCommand( char *command ) {

	// ajax commands from web
	char *buffer = malloc( 1000 );
	if ( 0 == strcmp( "vDet", command ) ) {
//		printf( "  Got vDet, read ADC for vDet on pin 1\n" );
		int adc1 = readAIO( vDet );
		sprintf( buffer, "%d", adc1 & 0x03FF );
		return buffer;
	}

	if ( 0 == strcmp( "iSense", command ) ) {
//		printf( "  Got iSense, detect signal for iSense on pin 9\n" );
		int pin9 = inputPin( iSense );
		sprintf( buffer, "%d", pin9 );
		return buffer;
	}

	int sz = 0; // = sprintf( buffer, html_head );	// Start with html and head /head tags and opening body tag
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
		free( buffer );
		return NULL;
	}
	sz += sprintf( &buffer[sz], command );
	return buffer;
}


// End of parser.c
