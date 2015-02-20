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
//#include "../commands/parser.h"

// If zombie/daemon, do not start thread to get user input
// So, off for use as app, on for use as service started at startup
//#define	BECOME_ZOMBIE

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
			nlog( ERROR, "Bad top directory:", baseDirectory, 71);	// returns failure to shell
		if ( port < 0 || port > 60000 )
			nlog( ERROR, "Invalid port number (range 1..60000):", argv[1], 72 );
	} else {	// Not valid arguments, use defaults
		port = 80;
//		baseDirectory = "/opt/ea-web/www";
	    sprintf( baseDirectory, "%s/www", rootDirectory );
	}

	if ( chdir( baseDirectory ) == -1 ) {
		nlog( ERROR, "Can't Change to directory:", baseDirectory, 73 );
	}

	running = 1; 						// Enable run loop
    timeCheck = DEFAULT_LOOP_TIME;		// Interval for checking in the loop

    int s = pthread_attr_init( &attr );
    if (s != 0)
		nlog( ERROR, "pthread_attr_init", "failed", 74 );

    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

#ifndef	BECOME_ZOMBIE

//	if we do not become a daemon, create a thread to monitor and respond to user input
	// we want to start a new thread to monitor and execute user command input
	// this only makes sense if we are not a zombie/daemon
	int resultUser = pthread_create( &pThreadUser, &attr, monitorUserOps, "unused" );
	if ( 0 != resultUser )
		nlog( ERROR, "creating thread for user input", "failed", 75 );	// returns failure to shell

#endif	// BECOME_ZOMBIE

	// we want to start a new thread to monitor our timed processes - like 'blink'
	int resultTime = pthread_create( &pThreadTime, &attr, monitorTimeOps, "unused" );
	if ( 0 != resultTime )
		nlog( ERROR, "create thread for timed operations", "failed", 76 ); // returns failure to shell

	int resultServer = pthread_create( &pThreadServer, &attr, monitorWebOps, "unused" );
	if ( 0 != resultServer )
		nlog( ERROR, "create thread for web operations", "failed", 77 ); // returns failure to shell

	monitorServiceHealth();

	s = pthread_attr_destroy( &attr );
    if (s != 0)
		nlog( ERROR, "pthread_attr_destroy", "failed", 78 );
	printf("\n\nDone running in main routine - normal exit\n\n");	// Should not happen unless failure
	return 0;
}


// End of nweb
