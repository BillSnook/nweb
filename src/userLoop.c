//
// userLoop.c
//
//  Created on: Dec 5, 2014
//      Author: Bill
//


#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>

#include "userLoop.h"
#include "../commands/parser.h"


void *monitorUserOps( void *arg ) {

//	(void)printf( "printf in monitorUserOps\n" );
//	char *msg = arg;
//	fprintf(stdout, msg );

	while ( 1 ) {
		printf( "\nCommand: " );
		scanf( "%s", command );
		printf( "\n\nGot command: %s\n\n", command );

		if ( 0 == strcmp( "exit", command ) ) {		// If command to terminate this program
//			pthread_exit( NULL );					// Kill thread
			exit( 1 );								// Or kill program
		}
		if ( 0 == strcmp( "guppy", command ) ) {
			printf( "\n\nGot guppy !!\n\n" );
		}
	}

	return NULL;
}


// End of userLoop.c
