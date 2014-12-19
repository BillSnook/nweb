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

#include "../utilities/nwInterface.h"

#ifdef	ENABLE_IO
extern	mraa_gpio_context gpio;
#endif	// ENABLE_IO


//--	----	----	----


char *parseCommand( char *command ) {

	if ( 0 == strcmp( "toggle", command ) ) {
		printf( "\n\n  Got toggle !!\n\n" );
#ifdef	ENABLE_IO
		togglePin( gpio );
		return NULL;
#else	// ENABLE_IO
		return NULL;
#endif	// ENABLE_IO
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
		exit( 1 );									// Kill program
	}

	if ( 0 == strcmp( "sample1", command ) ) {	// null command if empty uri entered

		char *buffer = malloc( 1000 );
		int sz = 0; // = sprintf( buffer, html_head );		// Start with html and head /head tags and opening body tag

		// Here we sprintf the html contents for display
		sz += sprintf( &buffer[sz], "Sample data from parser for command sample1" );

//		sz += sprintf( &buffer[sz], html_foot );	// String with ending /body and /html tags, finalize the page
		return buffer;
	}

	printf( "  Not recognized in parseCommand: %s\n", command );

	return NULL;
}

// End of parser.c
