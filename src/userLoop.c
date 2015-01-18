//
// userLoop.c
//
//  Created on: Dec 5, 2014
//      Author: Bill
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "userLoop.h"
#include "../commands/parser.h"

// Thread master, loop prompting for a command and then parsing it, rinse, repeat;
// ToDo: spawn thread to parse command to achieve better performance
void *monitorUserOps( void *arg ) {

	printf( "Starting user command input\n" );
//	char *msg = arg;
//	printf( msg );

	sleep( 1 );		// Let other threads initialize before presenting command prompt

	size_t sz = 256;
	char *cmd;
	ssize_t gotCount;
	while ( 1 ) {
//		printf( "\nCommand: " );
//		int scanSize = scanf( "%s\n", command );
		cmd = malloc( sz );
		gotCount = getline( &cmd, &sz, stdin );
		printf( "Got command size: %d, %s\n", gotCount, cmd );

		if ( gotCount > 1 ) {
			memmove( command, cmd, strlen( cmd ) - 1 );
			command[ strlen( cmd ) - 1 ] = 0;
			char *response = parseCommand( command );
			free( cmd );
			if ( NULL != response ) {
				printf( ">>  %s\n", response );
				free( response );
			}
		}
	}

	return NULL;
}


// End of userLoop.c
