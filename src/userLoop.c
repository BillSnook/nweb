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

extern int		running;


// Thread master, loop prompting for a command and then parsing it, rinse, repeat;
// ToDo: spawn thread to parse command to achieve better performance
void *monitorUserOps( void *arg ) {

	printf( "Starting user command input\n" );
//	char *msg = arg;
//	printf( msg );

//	sleep( 1 );		// Let other threads initialize before presenting command prompt

	size_t sz = 256;
	char *cmd = malloc( sz );
	ssize_t gotCount;
	while ( running ) {
//		printf( "\nCommand: " );
//		int scanSize = scanf( "%s\n", command );

		gotCount = getline( &cmd, &sz, stdin );	// includes newline and terminating NULL

		if ( gotCount > 1 ) {
//			printf( "Got command size: %d, %s\n", gotCount, cmd );
			memmove( command, cmd, strlen( cmd ) - 1 );
			command[ strlen( cmd ) - 1 ] = 0;	// replace newline with end of string null (0)
//			printf( "received user command to parse: %s\n", command );
			char *response = parseCommand( command );
			if ( NULL != response ) {
				printf( ">>  response: %s\n", response );
				free( response );
			}
		}
	}
	free( cmd );

	return NULL;
}


// End of userLoop.c
