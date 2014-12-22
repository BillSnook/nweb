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
        printf( "  Failed initing gpio\n" );
        mraa_result_print( MRAA_ERROR_UNSPECIFIED );
    	return 0;
//    } else {
//        printf( "  Inited gpio: %p\n", gpio );
    }

//    response = mraa_gpio_dir( gpio, MRAA_GPIO_OUT );
//    if (response != MRAA_SUCCESS) {
//        printf( "  Failed setting gpio pin direction\n" );
//        mraa_result_print((mraa_result_t) response);
//        return 0;
//    }

   	return pwmo;
}


void closePWMO( mraa_pwm_context pwmo ) {

	mraa_pwm_close( pwmo );
}


void isr( void *arg ) {

	double thisTime = getTimeCheck();
	double diff = ((double) (lastTime - thisTime));
	printf( "Interrupt, diff: %f", diff );
	lastTime = thisTime;
}


// mraa_result_t mraa_gpio_isr(mraa_gpio_context dev, gpio_edge_t edge, void (*fptr)(void *), void * args);

// mraa_result_t mraa_gpio_isr_exit(mraa_gpio_context dev);

mraa_result_t setupISRO( int pinNumber ) {

    on = 0;

    isro = mraa_gpio_init( pinNumber );
    if ( ! isro ) {
        printf( "  Failed initing isro\n" );
        mraa_result_print( MRAA_ERROR_UNSPECIFIED );
    	return 0;
//    } else {
//        printf( "  Inited isro: %p\n", gpio );
    }

    response = mraa_gpio_dir( isro, MRAA_GPIO_IN );
    if (response != MRAA_SUCCESS) {
        printf( "  Failed setting isro pin direction\n" );
        mraa_result_print((mraa_result_t) response);
        return 0;
    }
   	return mraa_gpio_isr( isro, MRAA_GPIO_EDGE_BOTH, &isr, NULL);
}


void closeISRO( mraa_gpio_context isro ) {

	mraa_gpio_isr_exit( isro );
}



// Finally done, exiting
void closeMRAA( void ) {

	mraa_deinit();
}

#endif	// ENABLE_IO

// End of nwInterface.c

