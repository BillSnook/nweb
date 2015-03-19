//
//  main.c
//  nweb
//
//  Created by Bill Snook on 11/27/14.
//  Copyright (c) 2014 Bill Snook. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>

#include <pthread.h>
//#include <semaphore.h>

#include "userLoop.h"
#include "timeLoop.h"
#include "serverThread.h"
#include "../commands/parser.h"


//--	----	----	----	----	----	----	----

extern char		*rootDirectory;
extern char		*baseDirectory;
extern int		port;

pthread_t		pThreadUser;	// this is our userLoop console input handler thread identifier
pthread_t		pThreadTime;	// this is our timeLoop timed process handler thread identifier
pthread_t		pThreadServer;	// this is our webLoop web server request handler thread identifier

//--	----	----	----	----	----	----	----

int main(int argc, char **argv) {

    pthread_attr_t attr;

	rootDirectory = argv[0];
//	printf( "\nIn main at start, rootDirectory: %s\n", rootDirectory );

	if ( argc > 3 || !strcmp(argv[1], "-?") || !strcmp(argv[1], "-h") ) {
		printWebHelp();
		exit( 0 );
	}

	if ( argc == 3 ) {
		port = atoi(argv[1]);
		baseDirectory = argv[2];
		if ( 0 == webDirectoryCheck( baseDirectory ) )	// if top level directory where user should never go
			nlog( ERROR, "Bad top directory:", baseDirectory, SH_ERROR_BAD_DIRECTORY);	// returns failure to shell
		if ( port < 0 || port > 60000 )
			nlog( ERROR, "Invalid port number (range 1..60000):", argv[1], SH_ERROR_BAD_PORT );
	} else {	// Not valid arguments, use defaults
		port = 80;
//		baseDirectory = "/opt/ea-web/www";
	    sprintf( baseDirectory, "%s/www", rootDirectory );
	}

	if ( chdir( baseDirectory ) == -1 ) {
		nlog( ERROR, "Can't Change to directory:", baseDirectory, SH_ERROR_INVALID_DIRECTORY );
	}

	running = 1; 						// Enable run loop
    timeCheck = DEFAULT_LOOP_TIME;		// Interval for checking in the loop

    int s = pthread_attr_init( &attr );
    if (s != 0)
		nlog( ERROR, "pthread_attr_init", "failed", SH_ERROR_PTHREAD_CREATE_ATTR );

    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );


	// we want to start a new thread to monitor and execute user input command
    // we should only do this if we are not running as a system service - how to detect this?
	int resultUser = pthread_create( &pThreadUser, &attr, monitorUserOps, NULL );
	if ( 0 != resultUser )
		nlog( ERROR, "creating thread for user input", "failed", SH_ERROR_PTHREAD_CREATE_USER );	// returns failure to shell


	// we want to start a new thread to monitor our timed processes - like 'blink'
	int resultTime = pthread_create( &pThreadTime, &attr, monitorTimeOps, NULL );
	if ( 0 != resultTime )
		nlog( ERROR, "create thread for timed operations", "failed", SH_ERROR_PTHREAD_CREATE_TIME ); // returns failure to shell


	// we want start a thread to monitor a web socket to handle web requests to an http server
	int resultServer = pthread_create( &pThreadServer, &attr, monitorWebOps, NULL );
	if ( 0 != resultServer )
		nlog( ERROR, "create thread for web operations", "failed", SH_ERROR_PTHREAD_CREATE_WEB ); // returns failure to shell


	// we want to monitor our threads to monitor their status
	monitorServiceHealth();

	s = pthread_attr_destroy( &attr );
    if (s != 0)
		nlog( ERROR, "pthread_attr_destroy", "failed", SH_ERROR_PTHREAD_DESTROY_ATTR );
	printf("\n\nDone running in main routine - normal exit\n\n");	// Should not happen unless failure
	return 0;
}


// End of nweb
