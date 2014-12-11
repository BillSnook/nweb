//
// nwInterface.c
//
//  Created on: Nov 30, 2014
//      Author: Bill
//


#include <stdio.h>

#include "nwInterface.h"

#ifdef	ENABLE_IO

int response;
int iopin;
int on;

int adc_value;

extern	mraa_gpio_context gpio;


void startMRAA( void ) {

	mraa_init();
}


mraa_gpio_context setupGPIO( int pinNumber ) {

//    printf( "\n  Hello mraa\n" );
//    printf( "  mraa Version: %s\n", mraa_get_version() );

//    mraa_platform_t platform = mraa_get_platform_type();
//    printf( "  Platform type: %d\n", platform );

    on = 0;

    iopin = pinNumber;

    gpio = mraa_gpio_init( iopin );
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


void closeMRAA( void ) {

	mraa_deinit();
}

#endif	// ENABLE_IO

// End of nwInterface.c

