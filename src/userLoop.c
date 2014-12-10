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

	(void)printf( "Starting user command input\n" );
//	char *msg = arg;
//	fprintf(stdout, msg );

	sleep( 1 );		// Let other threads initialize before presenting command prompt

	while ( 1 ) {
		printf( "\nCommand: " );
		scanf( "%s", command );
		printf( "\nGot command: %s\n", command );

		parseCommand( command );
	}

	return NULL;
}


// End of userLoop.c
