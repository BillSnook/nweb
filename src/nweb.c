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


char	*baseDirectory;


int main(int argc, char **argv) {
	int port, listenfd, requestfd, bound;
	size_t length;
	static struct sockaddr_in reply_socketaddr;	// receive socket address from accept routine
	static struct sockaddr_in listen_socketaddr;	// listen socket socket address

	if ( argc > 3 || !strcmp(argv[1], "-?") || !strcmp(argv[1], "-h") ) {
		printWebHelp();
		exit( 0 );
	}

	if ( argc == 3 ) {
		port = atoi(argv[1]);
		baseDirectory = argv[2];
		if ( 0 == webDirectoryCheck( baseDirectory ) ) {	// if top level directory where user should never go
			printf("ERROR: Bad top directory %s, see nweb -?\n", baseDirectory );
			exit(3);
		}
	} else {
		port = 80;
		baseDirectory = "/opt/ea-web/web_src";
	}

	if ( chdir(baseDirectory) == -1 ) {
		printf("ERROR: Can't Change to directory in second argument: %s\n", baseDirectory);
		exit(4);
	}

	running = 1; 						// Enable run loop
    timeCheck = 1.0;					// Interval for checking in the loop


#ifndef	BECOME_ZOMBIE
/*	Not if we become a daemon */
	// we want to start a new thread to monitor and execute user command input
	// this only makes sense if we are not a zombie/daemon
	pthread_t pThreadUser;	// this is our thread identifier
	int resultUser = pthread_create( &pThreadUser, NULL, monitorUserOps, "param2" );
	if ( 0 != resultUser ) {
//		printf( "\n\npthread_create 2 error. Ack!!\n\n" );
		nlog( ERROR, "system call", "pthread_create for user input", 0 );	// returns failure to shell
	}
/* */
#endif	// BECOME_ZOMBIE

#ifndef	BECOME_ZOMBIE
	// we want to start a new thread to monitor our timed processes - like 'blink'
	pthread_t pThreadTime;	// this is our thread identifier
	int resultTime = pthread_create( &pThreadTime, NULL, monitorTimeOps, "param1" );
	if ( 0 != resultTime ) {
		printf( "\n\npthread_create 1 error. Ack!!\n\n" );
		nlog( ERROR, "system call", "pthread_create for timed operations", 0 );	// returns failure to shell
	}
#endif	// BECOME_ZOMBIE

	// setup the network socket
	if ( ( listenfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		nlog( ERROR, "system call", "socket", 0 );

	if ( port < 0 || port > 60000 )
		nlog( ERROR, "Invalid port number (try 1->60000)", argv[1], 0 );

	listen_socketaddr.sin_family = AF_INET;
	listen_socketaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	listen_socketaddr.sin_port = htons( port );

	bound = bind( listenfd, (struct sockaddr *)&listen_socketaddr, sizeof(listen_socketaddr)); // permission error
	if ( bound < 0 )
		nlog( ERROR, "system call", "bind", 0 );

	if ( listen( listenfd, 64 ) < 0 )
		nlog( ERROR, "system call", "listen", 0 );

#ifndef	BECOME_ZOMBIE
	printf( "\nStarting web server process, version %d.%d\n", VERSION, SUB_VERSION );
#endif	// BECOME_ZOMBIE

	pthread_t pThread;	// this is our thread identifier
	while ( running ) {
		length = sizeof( reply_socketaddr );
		if ( ( requestfd = accept( listenfd, (struct sockaddr *)&reply_socketaddr, &length) ) < 0 )
			nlog( ERROR, "system call", "accept", 0 );

		web_data *webData = malloc( sizeof( web_data ) );
		if ( webData ) {
			webData->socketfd = requestfd;
			webData->baseDirectory = baseDirectory;

			int result = pthread_create( &pThread, NULL, webService, webData );
			if ( 0 != result ) {
				nlog( ERROR, "system call", "pthread_create x", 0 );
			}
		} else {
			nlog( ERROR, "system call", "allocate web_data", 0 );
		}
	}
	printf("\n\nNot running in main routine\n\n");
	return 0;
}

// End of nweb
