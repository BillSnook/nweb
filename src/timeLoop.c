//
// timeLoop.c
//
//  Created on: Dec 5, 2014
//      Author: Bill
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <signal.h>

#include "timeLoop.h"
//#include "../commands/parser.h"
#include "../utilities/nwTime.h"


// Enable to use i/o code on Edison Arduino breakout board, disable to run on Edison breakout board
#define	ENABLE_IO


#ifdef	ENABLE_IO

#include "../utilities/nwInterface.h"

#define	DEFAULT_LOOP_TIME	1.0

#else	// ENABLE_IO

#define	DEFAULT_LOOP_TIME	10.0

#endif	// ENABLE_IO


int		running = 0;


void *monitorTimeOps( void *arg ) {

	(void)printf( "Start real-time action control\n" );
//	char *msg = arg;
//	fprintf(stdout, msg );

//	fprintf(stdout, "\n\n    nweb/MotionKit Version %d.%d, starting loop process\n", VERSION, SUB_VERSION );

    signal(SIGINT, sig_handler);

    double timeCheck = DEFAULT_LOOP_TIME;	// Interval for ops in the loop

#ifdef	ENABLE_IO
	setupGPIO( 13 );
#endif	// ENABLE_IO

	startElapsedTime();
    while ( running == 0 ) {
    	if ( getElapsedTime() > timeCheck ) {
    		startElapsedTime();

#ifdef	ENABLE_IO
//    		togglePin();
#else	// ENABLE_IO
//        	fprintf(stdout, "\n    Tick\n" );
#endif	// ENABLE_IO

    	}
    }

#ifdef	ENABLE_IO
    closeGPIO();
#endif	// ENABLE_IO

	exit( 1 );						// Exit program when told to quit via cntl-C

	return NULL;
}


void sig_handler(int signo) {
    if (signo == SIGINT) {
        printf("\n\n        Closing IO nicely\n\n");
        running = -1;
    }
}


// End of timeLoop.c
