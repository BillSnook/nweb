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


void *monitorUserOps( void *arg ) {

//	(void)printf( "printf in monitorUserOps\n" );
//	char *msg = arg;
//	fprintf(stdout, msg );

	sleep( 1 );

	while ( 1 ) {
		printf( "\nCommand: " );
		scanf( "%s", command );
		printf( "\n\nGot command: %s\n\n", command );

		parseCommand( command );
	}

	return NULL;
}


// End of userLoop.c
