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


#ifdef	DISABLE_IO

#define	DEFAULT_LOOP_TIME	10.0

#else	// DISABLE_IO

#include "../utilities/nwInterface.h"

extern	mraa_gpio_context gpio;
//extern	mraa_gpio_context isro;
//extern	mraa_pwm_context  pwmo;

extern	mraa_aio_context	vDet;
extern	mraa_gpio_context	iSense;

#define	DEFAULT_LOOP_TIME	1.0

#endif	// DISABLE_IO


extern int		running;


void *monitorTimeOps( void *arg ) {

//	printf( "Start real-time action control\n" );
//	char *msg = arg;
//	printf( msg );

//	printf( "\n\n    nweb/MotionKit Version %d.%d, starting loop process\n", VERSION, SUB_VERSION );

	running = 0; 							// Enable run loop

    signal(SIGINT, sig_handler);

    double timeCheck = DEFAULT_LOOP_TIME;	// Interval for ops in the loop

#ifndef	DISABLE_IO
//    double dutyCycle = 0.2;					// Portion of time output is on
//    mraa_gpio_context gpio;
//    mraa_gpio_context isro;
//    mraa_pwm_context  pwmo;

    startMRAA();
    gpio = setupGPIOOut( 13 );
    iSense = setupGPIOIn( 9 );
    vDet = setupAIO( 0 );
//    isro = setupISRO( 12 );
//    pwmo = setupPWMO( 3 );
#endif	// DISABLE_IO

	startElapsedTime();
    while ( running == 0 ) {
    	if ( getElapsedTime() > timeCheck ) {
    		startElapsedTime();

#ifdef	DISABLE_IO
//        	printf( "\n    Tick\n" );
#else	// DISABLE_IO

    		togglePin( gpio );
//    		setDutyCycle( pwmo, dutyCycle );
//    		dutyCycle = 1.0 - dutyCycle;

#endif	// DISABLE_IO

    	}
    }

#ifndef	DISABLE_IO
//    closePWMO( pwmo );
//    closeISRO( isro );
    closeAIO( vDet );
    closeGPIO( iSense );
    closeGPIO( gpio );

    closeMRAA();
#endif	// DISABLE_IO

	exit( 1 );						// Exit program when told to quit via cntl-C or by command

	return NULL;
}


void sig_handler(int signo) {
    if (signo == SIGINT) {			// Typically control-C
//        printf("\n\n        Closing IO nicely\n\n");
        running = -1;
    }
}


// End of timeLoop.c
