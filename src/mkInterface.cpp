//============================================================================
// Name        : mkInterface.hpp
// Author      : Bill Snook
// Version     : 0.1
// Created on  : Nov 20, 2014
// Copyright   :
// Description : Base for MotionKit
//============================================================================

#include <stdio.h>

#include "mkInterface.hpp"

#include "mraa.hpp"


mraa::Gpio* gpio;


mkInterface::mkInterface() {

    response = 0;
    on = 0;
}


mkInterface::~mkInterface() {

    if ( gpio )
    	delete gpio;

    mraa_deinit();
    fprintf(stdout, "\nGoodbye mraa\n" );

}


bool mkInterface::setupMRAA( int pinNumber ) {

    on = 0;

    iopin = pinNumber;
    gpio = new mraa::Gpio( iopin, true, false );

    fprintf(stdout, "\nHello mraa\n  Version: %s", mraa_get_version());

    mraa_platform_t platform = mraa_get_platform_type();
    fprintf(stdout, "\n  Platform type: %d\n", platform );

    response = gpio->dir(mraa::DIR_OUT);
    if (response != MRAA_SUCCESS) {
        mraa_result_print((mraa_result_t) response);
        return false;
    }
   	return true;
}


void mkInterface::togglePin() {

    response = gpio->write( on );
    on = ( 0 == on ) ? 1 : 0;
}



/*
//	  adc input setup
//    uint16_t adc_value;
//    mraa::Aio* a0;

//    a0 = new mraa::Aio(0);
//    if (!a0) {
//        return MRAA_ERROR_UNSPECIFIED;
//    }

*/

/*
//			adc read
//   		adc_value = a0->read();
//   		fprintf(stdout, "ADC A0 read %X - %d\n", adc_value, adc_value);

*/
