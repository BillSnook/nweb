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

#include "../src/common.h"
#include "parser.h"

#include "../utilities/nwInterface.h"

#ifdef	ENABLE_IO
extern	mraa_gpio_context gpio;
#endif	// ENABLE_IO


//--	----	----	----


int parseCommand( char *command ) {

	if ( 0 == strcmp( "toggle", command ) ) {
		printf( "\n\n  Got toggle !!\n\n" );
#ifdef	ENABLE_IO
		togglePin( gpio );
		return 1;
#else	// ENABLE_IO
		return 0;
#endif	// ENABLE_IO
	}

	if ( 0 == strcmp( "guppy", command ) ) {	// test
		printf( "\n\n  Got guppy !!\n\n" );
		return 1;
	}

	if ( 0 == strcmp( "moosetrap", command ) ) {	// test
		printf( "\n\n  Got moosetrap !!\n\n" );
		return 1;
	}

	if ( 0 == strcmp( "exit", command ) ) {		// If command to terminate this program is entered
		printf( "\n\n  Got exit !!\n\n" );
//		pthread_exit( NULL );					// Kill thread
		exit( 1 );								// Kill program
	}

	printf( "  Not recognized in parseCommand: %s\n", command );

	return 0;
}

// End of parser.c
