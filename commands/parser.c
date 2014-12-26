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


char *parseCommand( char *command ) {

	if ( 0 == strcmp( "toggle", command ) ) {
		printf( "\n\n  Got toggle !!\n\n" );
#ifdef	DISABLE_IO
		return NULL;
#else	// DISABLE_IO
		togglePin( gpio );
		return NULL;
#endif	// DISABLE_IO
	}

	if ( 0 == strcmp( "guppy", command ) ) {		// test
		printf( "\n  Got guppy !!\n" );
		return NULL;
	}

	if ( 0 == strcmp( "moosetrap", command ) ) {	// null command if empty uri entered
		printf( "\n  Got moosetrap !!\n" );
		return NULL;
	}

	if ( 0 == strcmp( "exit", command ) ) {			// If command to terminate this program is entered
		printf( "\n\n  Got exit !!\n\n" );
//		pthread_exit( NULL );						// Kill thread
//		exit( 1 );									// Kill program
		running = 1;
		return NULL;
	}

	if ( 0 == strcmp( "sample1", command ) ) {	// null command if empty uri entered

		printf( "  parse sample1\n" );
		char *buffer = malloc( 1000 );
		int sz = 0; // = sprintf( buffer, html_head );		// Start with html and head /head tags and opening body tag

		// Here we sprintf the html contents for display
		sz += sprintf( &buffer[sz], "Data from parser" );

//		sz += sprintf( &buffer[sz], html_foot );	// String with ending /body and /html tags, finalize the page
		return buffer;
	}

	if ( 0 == strcmp( "adcData1", command ) ) {	// null command if empty uri entered
		printf( "  Got adcData1\n" );
		return getADCData();
	}

	printf( "  command not recognized in parseCommand: %s\n", command );

	return NULL;
}


// End of parser.c
