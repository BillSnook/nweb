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


#ifndef	DISABLE_IO

#include "../utilities/nwInterface.h"

extern	mraa_gpio_context gpio;
//extern	mraa_gpio_context isro;
//extern	mraa_pwm_context  pwmo;

extern	mraa_aio_context	vDet;
extern	mraa_gpio_context	iSense;

#endif	// DISABLE_IO


extern int		running;
extern double	timeCheck;


void isrProc( void *arg ) {

//	mraa_gpio_context isro = (mraa_gpio_context)arg;
//	double thisTime = getTimeCheck();
//	double diff = ((double) (thisTime - lastTime) * 2);

	int readInput =  mraa_gpio_read( isro );
	printf( "isrProc, state: %d\n", readInput );

//	printf( "Interrupt1, state: %d, diff: %.2f uSec\n", readInput, diff );
//	lastTime = thisTime;

}


void *monitorTimeOps( void *arg ) {

//	char *msg = arg;
//	printf( msg );

    signal(SIGINT, sig_handler);

#ifndef	DISABLE_IO
//    double dutyCycle = 0.2;					// Portion of time output is on
//    mraa_gpio_context gpio;
//    mraa_gpio_context isro;
//    mraa_pwm_context  pwmo;

    startMRAA();
    gpio = setupGPIOOut( 13 );
    iSense = setupGPIOIn( 9 );
    vDet = setupAIO( 0 );
//    isro = setupISRO( 12, &isrProc );
//    pwmo = setupPWMO( 3 );
#endif	// DISABLE_IO

	printf( "Start real-time timed action control, ready to start loop, running: %d, timeCheck: %f\n", running, timeCheck );

	startElapsedTime();
    while ( running ) {
    	if ( getElapsedTime() > timeCheck ) {
    		startElapsedTime();

#ifdef	DISABLE_IO
        	printf( "\n    Tick\n" );
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
        running = 0;
    }
}


// End of timeLoop.c
