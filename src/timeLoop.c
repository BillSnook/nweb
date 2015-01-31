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
extern	mraa_gpio_context isro;
//extern	mraa_pwm_context  pwmo;

extern	mraa_aio_context	vDet;
extern	mraa_gpio_context	iSense;

//mraa_gpio_context uartRx;
//mraa_gpio_context uartTx;
mraa_uart_context uart1;

#endif	// DISABLE_IO


extern double	timeCheck;
extern int		running;
extern int		showMonitor;

extern int		userLoopExitCode;
extern int		timeLoopExitCode;
extern int		servLoopExitCode;
extern int		webLoopExitCode;

//--	----	----	----	----	----	----	----

void *monitorTimeOps( void *arg ) {

	timeLoopExitCode = 0;
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

//    uartRx = setupGPIOIn( 0 );
//    uartTx = setupGPIOOut( 1 );

    uart1 = mraa_uart_init( 1 );

#endif	// DISABLE_IO

    showMonitor = 0;	// off for now

	printf( "Starting real-time timed action control, ready to start loop, running: %d, timeCheck: %f\n", running, timeCheck );

//	startElapsedTime();
    while ( running ) {
		if ( ++timeLoopExitCode > 99 )
			timeLoopExitCode = 0;
//    	if ( getElapsedTime() > timeCheck ) {
//    		startElapsedTime();

#ifdef	DISABLE_IO
        	printf( "\n    Tick\n" );
#else	// DISABLE_IO

    		togglePin( gpio );
//    		setDutyCycle( pwmo, dutyCycle );
//    		dutyCycle = 1.0 - dutyCycle;

#endif	// DISABLE_IO

//    	}
    	usleep( 100000 );
    }

#ifndef	DISABLE_IO
//    closeGPIO( uartTx );
//    closeGPIO( uartRx );
//    closePWMO( pwmo );
//    closeISRO( isro );
    closeAIO( vDet );
    closeGPIO( iSense );
    closeGPIO( gpio );

    closeMRAA();
#endif	// DISABLE_IO

//	exit( 1 );						// Exit program when told to quit via cntl-C or by command
    timeLoopExitCode = 30;

	return NULL;
}

//--	----	----	----	----	----	----	----

void sig_handler(int signo) {
    printf("\n\n        Signal handler, got signal\n\n");
    if (signo == SIGINT) {			// Typically control-C
        printf("\n\n        Signal handler, shutting down\n\n");
        running = 0;
    }
}

//--	----	----	----	----	----	----	----

void isrProc( void *arg ) {

//	mraa_gpio_context isro = (mraa_gpio_context)arg;
//	double thisTime = getTimeCheck();
//	double diff = ((double) (thisTime - lastTime) * 2);

	int readInput =  mraa_gpio_read( isro );
	printf( "isrProc, state: %d\n", readInput );

//	printf( "Interrupt1, state: %d, diff: %.2f uSec\n", readInput, diff );
//	lastTime = thisTime;

}

//--	----	----	----	----	----	----	----

void monitorServiceHealth( void ) {

	// wait a while for threads to stop
	int looper = 0;
	while ( running ) {
		sleep( 1 );
		if ( ++looper >= 10 ) {
			if ( showMonitor ) {
				displayServiceHealth();
			}
			looper = 0;
		}
	}
	displayServiceHealth();
	sleep( 1 );
}

//--	----	----	----	----	----	----	----

void displayServiceHealth( void ) {

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

//	printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	printf( "%02d:%02d:%02d SystemHealth monitor thread status, user: %d, time: %d, server: %d, web: %d\n", tm.tm_hour, tm.tm_min, tm.tm_sec, userLoopExitCode, timeLoopExitCode, servLoopExitCode, webLoopExitCode );
}


// End of timeLoop.c
