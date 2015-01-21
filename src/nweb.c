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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
//#include <semaphore.h>


// If zombie/daemon, do not start thread to get user input
// So, off for use as app, on for use as service started at startup
//#define	BECOME_ZOMBIE


#include "userLoop.h"
#include "timeLoop.h"
#include "serverThread.h"
//#include "../commands/parser.h"


//extern int				running;


//--	----	----	----	----	----	----	----


char	*rootDirectory;
char	*baseDirectory;


int main(int argc, char **argv) {
	int port, listenfd, requestfd, bound;
	size_t length;
	static struct sockaddr_in reply_socketaddr;		// receive socket address from accept routine
	static struct sockaddr_in listen_socketaddr;	// listen socket socket address

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

	} else {
		port = 80;
		baseDirectory = "/opt/ea-web/web_src";
	}

	if ( chdir( baseDirectory ) == -1 ) {
		nlog( ERROR, "Can't Change to directory:", baseDirectory, 73 );
	}

	running = 1; 						// Enable run loop
    timeCheck = DEFAULT_LOOP_TIME;		// Interval for checking in the loop

	pthread_t pThread;	// this is our thread identifier
    pthread_attr_t attr;
    int s = pthread_attr_init( &attr );
    if (s != 0)
		nlog( ERROR, "pthread_attr_init", "failed", 79 );

    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

#ifndef	BECOME_ZOMBIE
//	if we do not become a daemon, create a thread to monitor and respond to user input
	// we want to start a new thread to monitor and execute user command input
	// this only makes sense if we are not a zombie/daemon
	pthread_t pThreadUser;	// this is our thread identifier
	int resultUser = pthread_create( &pThreadUser, &attr, monitorUserOps, "param2" );
	if ( 0 != resultUser )
		nlog( ERROR, "creating thread for user input", "failed", 75 );	// returns failure to shell

#endif	// BECOME_ZOMBIE

	// we want to start a new thread to monitor our timed processes - like 'blink'
	pthread_t pThreadTime;	// this is our thread identifier
	int resultTime = pthread_create( &pThreadTime, &attr, monitorTimeOps, "param1" );
	if ( 0 != resultTime )
		nlog( ERROR, "create thread for timed operations", "failed", 76 ); // returns failure to shell

	// setup the network socket
	if ( ( listenfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		nlog( ERROR, "creating new socket", "failed", 77 );

	listen_socketaddr.sin_family = AF_INET;
	listen_socketaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	listen_socketaddr.sin_port = htons( port );

	bound = bind( listenfd, (struct sockaddr *)&listen_socketaddr, sizeof(listen_socketaddr)); // permission error
	if ( bound < 0 )
		nlog( ERROR, "binding to socket", "failed", 78 );

	if ( listen( listenfd, 64 ) < 0 )
		nlog( ERROR, "creating listener socket", "failed", 78 );

#ifndef	BECOME_ZOMBIE
	printf( "\nStarting web server process, version %d.%d\n", VERSION, SUB_VERSION );
#endif	// BECOME_ZOMBIE

	while ( running ) {
		length = sizeof( reply_socketaddr );
		if ( ( requestfd = accept( listenfd, (struct sockaddr *)&reply_socketaddr, &length) ) < 0 ) {
			nlog( ERROR, "accepting request on listener", "failed", 80 );
		}

		web_data *webData = malloc( sizeof( web_data ) );
		if ( webData ) {
			webData->socketfd = requestfd;
			webData->baseDirectory = baseDirectory;

			int result = pthread_create( &pThread, &attr, webService, webData );
			if ( 0 != result ) {
				nlog( ERROR, "creating a thread to handle new request", "failed", 81 );
			}
		} else {
			nlog( ERROR, "allocating web_data struct for request", "failed", 82 );
		}
	}
    s = pthread_attr_destroy( &attr );
    if (s != 0)
		nlog( ERROR, "pthread_attr_destroy", "failed", 83 );
	printf("\n\nDone running in main routine - normal exit\n\n");
	return 0;
}


// End of nweb
