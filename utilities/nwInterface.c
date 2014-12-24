//
// nwInterface.c
//
//  Created on: Nov 30, 2014
//      Author: Bill
//


#include <stdio.h>

#include "nwInterface.h"
#include "nwTime.h"

#ifdef	ENABLE_IO

int response;
int iopin;
int on;
int isrState;

int adc_value;

double lastTime = 0.0;


extern	mraa_gpio_context gpio;
extern	mraa_gpio_context isro;
extern	mraa_pwm_context pwmo;


void startMRAA( void ) {

	printf( "\n  I/O is enabled\n" );
	mraa_init();

//    printf( "\n  Hello mraa\n" );
	printf( "  mraa Version: %s\n", mraa_get_version() );

//    mraa_platform_t platform = mraa_get_platform_type();
//    printf( "  Platform type: %d\n", platform );

}


mraa_gpio_context setupGPIO( int pinNumber ) {

    on = 0;

    gpio = mraa_gpio_init( pinNumber );
    if ( ! gpio ) {
        printf( "  Failed initing gpio\n" );
        mraa_result_print( MRAA_ERROR_UNSPECIFIED );
    	return 0;
//    } else {
//        printf( "  Inited gpio: %p\n", gpio );
    }

    response = mraa_gpio_dir( gpio, MRAA_GPIO_OUT );
    if (response != MRAA_SUCCESS) {
        printf( "  Failed setting gpio pin direction\n" );
        mraa_result_print((mraa_result_t) response);
        return 0;
    }

   	return gpio;
}


void togglePin(  mraa_gpio_context gpio ) {

	response = mraa_gpio_write( gpio, on );
    if (response != MRAA_SUCCESS) {
        printf( "\n  Failed writing to gpio pin: %p\n", gpio );
        mraa_result_print((mraa_result_t) response);
    }
    on = ( 0 == on ) ? 1 : 0;
}


void outputPin(  mraa_gpio_context gpio, int offOn ) {

	response = mraa_gpio_write( gpio, offOn );
    if (response != MRAA_SUCCESS) {
        mraa_result_print((mraa_result_t) response);
    }
}



void closeGPIO( mraa_gpio_context gpio ) {

	mraa_gpio_close( gpio );
}


mraa_pwm_context setupPWMO( int pinNumber ) {

    on = 0;

    pwmo = mraa_pwm_init( pinNumber );
    if ( ! pwmo ) {
        printf( "  Failed initing pwmo\n" );
        mraa_result_print( MRAA_ERROR_UNSPECIFIED );
    	return 0;
    } else {
        printf( "  Inited pwmo: %p\n", gpio );
    }

    response = mraa_gpio_dir( gpio, MRAA_GPIO_OUT );
    if (response != MRAA_SUCCESS) {
        printf( "  Failed setting pwmo pin direction\n" );
        mraa_result_print((mraa_result_t) response);
        return 0;
    }

   	return pwmo;
}


void closePWMO( mraa_pwm_context pwmo ) {

	mraa_pwm_close( pwmo );
}



// mraa_result_t mraa_gpio_isr(mraa_gpio_context dev, gpio_edge_t edge, void (*fptr)(void *), void * args);

// mraa_result_t mraa_gpio_isr_exit(mraa_gpio_context dev);

mraa_gpio_context setupISRO( int pinNumber ) {

    isro = mraa_gpio_init( pinNumber );
    if ( ! isro ) {
        printf( "  Failed initing isro\n" );
        mraa_result_print( MRAA_ERROR_UNSPECIFIED );
    	return 0;
    } else {
        printf( "  Inited isro on pin: %d\n", pinNumber );
    }

    response = mraa_gpio_dir( isro, MRAA_GPIO_IN );
    if (response != MRAA_SUCCESS) {
        printf( "  Failed setting isro pin direction\n" );
        mraa_result_print((mraa_result_t) response);
        return 0;
    }
    printf( "  Setup isro pin direction to IN\n" );
    mraa_result_t result = mraa_gpio_isr( isro, MRAA_GPIO_EDGE_BOTH, &isr1, NULL);

    if ( MRAA_SUCCESS == result ) {
        printf( "  Setup isro interrupt service routine\n" );

        {	// Init to test pin level
        lastTime = getTimeCheck();
        isr1( NULL );				// Check level
        }
    } else {
        printf( "  Setup isro interrupt service routine failed with result: %d\n", result );
    }

 	return isro;
}


void closeISRO( mraa_gpio_context isro ) {

	mraa_gpio_isr_exit( isro );
}


void isr1( void *arg ) {

	double thisTime = getTimeCheck();
	double diff = ((double) (thisTime - lastTime) * 2);

	int readInput =  mraa_gpio_read( isro );

	printf( "Interrupt1, state: %d, diff: %.2f uSec\n", readInput, diff );
	lastTime = thisTime;
}


// Finally done, exiting
void closeMRAA( void ) {

	mraa_deinit();
}

#endif	// ENABLE_IO

// End of nwInterface.c

