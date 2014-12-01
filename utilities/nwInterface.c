//
// nwInterface.c
//
//  Created on: Nov 30, 2014
//      Author: Bill
//

#include "nwInterface.h"
#include "mraa.h"


int response;
int iopin;
int on;

int adc_value;

mraa_gpio_context gpio;


int setupGPIO( int pinNumber ) {

    on = 0;

    iopin = pinNumber;

    gpio = mraa_gpio_init( iopin );
    if ( ! gpio ) {
        fprintf( stdout, "\n  Failed initing gpio\n" );
        mraa_result_print( MRAA_ERROR_UNSPECIFIED );
    	return 0;
    } else {
        fprintf( stdout, "\n  Inited gpio: %p\n", gpio );
    }

    fprintf( stdout, "\nHello mraa\n  Version: %s", mraa_get_version() );

    mraa_platform_t platform = mraa_get_platform_type();
    fprintf( stdout, "\n  Platform type: %d\n", platform );

    response = mraa_gpio_dir( gpio, MRAA_GPIO_OUT );
    if (response != MRAA_SUCCESS) {
        fprintf( stdout, "\n  Failed setting gpio pin direction\n" );
        mraa_result_print((mraa_result_t) response);
        return 0;
    }

   	return 1;
}


void togglePin() {

	response = mraa_gpio_write( gpio, on );
    if (response != MRAA_SUCCESS) {
        fprintf( stdout, "\n  Failed writing to gpio pin: %p\n", gpio );
        mraa_result_print((mraa_result_t) response);
    }
    on = ( 0 == on ) ? 1 : 0;
}


void outputPin( int offOn ) {

	response = mraa_gpio_write( gpio, offOn );
    if (response != MRAA_SUCCESS) {
        mraa_result_print((mraa_result_t) response);
    }
}
