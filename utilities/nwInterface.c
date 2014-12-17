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


// Finally done with it
void closeMRAA( void ) {

	mraa_deinit();
}

#endif	// ENABLE_IO

// End of nwInterface.c

