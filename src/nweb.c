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


// This uses fork to create a new untethered process, then halts and returns to the command line
// Otherwise if disabled, the program continues and we get printf output for debugging
// So, off for debugging, on to run as daemon in background
//#define	BECOME_ZOMBIE


#include "userLoop.h"
#include "timeLoop.h"
#include "serverThread.h"
//#include "../commands/parser.h"


//--	----	----	----	----	----	----	----


char	*baseDirectory;


int main(int argc, char **argv) {
	int port, listenfd, socketfd, hit, bound;
	size_t length;
//	char *str;
	static struct sockaddr_in cli_addr;		// static = initialised to zeros
	static struct sockaddr_in serv_addr;	// static = initialised to zeros

	if ( argc < 3 || argc > 3 || !strcmp(argv[1], "-?") ) {
		printWebHelp();
		exit( 0 );
	}

	baseDirectory = argv[2];
	if ( 0 == webDirectoryCheck( baseDirectory ) ) {	// if top level directory where user should never go
		printf("ERROR: Bad top directory %s, see nweb -?\n", baseDirectory );
		exit(3);
	}

	if ( chdir(argv[2]) == -1 ) {
		printf("ERROR: Can't Change to directory in second argument: %s\n",argv[2]);
		exit(4);
	}


#ifdef	BECOME_ZOMBIE
	// Create daemon + unstoppable and no zombie children (= no wait())
	int pid;
	if ((pid = fork()) < 0) {
		nlog( ERROR, "system call", "fork", 0 );
	} else {
		if ( pid > 0 ) {						// parent
			return 0;							// parent returns OK to shell
		}
	}

	// child
	for (i = 0; i < 32; i++ )
		close( i );						// close open files (such as i/o)

	setpgrp();							// break away from process group

	signal(SIGCHLD, SIG_IGN);				// ignore child death
	signal(SIGHUP, SIG_IGN);				// ignore terminal hangups

#endif	// BECOME_ZOMBIE

	// we want to start a new thread to monitor our timed processes - like 'blink'
	pthread_t pThreadTime;	// this is our thread identifier
	int resultTime = pthread_create( &pThreadTime, NULL, monitorTimeOps, "param1" );
	if ( 0 != resultTime ) {
		printf( "\n\npthread_create 1 error. Ack!!\n\n" );
		nlog( ERROR, "system call", "pthread_create 1", 0 );	// returns failure to shell
	}

	// we want to start a new thread to monitor our user input processes
	pthread_t pThreadUser;	// this is our thread identifier
	int resultUser = pthread_create( &pThreadUser, NULL, monitorUserOps, "param2" );
	if ( 0 != resultUser ) {
		printf( "\n\npthread_create 2 error. Ack!!\n\n" );
		nlog( ERROR, "system call", "pthread_create 2", 0 );	// returns failure to shell
	}

	// setup the network socket
	if ( ( listenfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		nlog( ERROR, "system call", "socket", 0 );

	port = atoi(argv[1]);
	if ( port < 0 || port > 60000 )
		nlog( ERROR, "Invalid port number (try 1->60000)", argv[1], 0 );

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	serv_addr.sin_port = htons( port );

	bound = bind( listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); // permission error
	if ( bound < 0 )
		nlog( ERROR, "system call", "bind", 0 );

	if ( listen( listenfd, 64 ) < 0 )
		nlog( ERROR, "system call", "listen", 0 );

	printf( "\nStarting web server process\n" );

	pthread_t pThread;	// this is our thread identifier
	for ( hit = 1; ; hit++ ) {
		length = sizeof( cli_addr );
		if ( ( socketfd = accept( listenfd, (struct sockaddr *)&cli_addr, &length) ) < 0 )
			nlog( ERROR, "system call", "accept", 0 );

		web_data *webData = malloc( sizeof( web_data ) );
		if ( webData ) {
			webData->socketfd = socketfd;
			webData->baseDirectory = baseDirectory;

			int result = pthread_create( &pThread, NULL, webService, webData );
			if ( 0 != result ) {
				nlog( ERROR, "system call", "pthread_create x", 0 );
			}
		} else {
			nlog( ERROR, "system call", "allocate web_data", 0 );
		}
	}
}

// End of nweb
