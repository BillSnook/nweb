//============================================================================
// Name        : mkInterface.hpp
// Author      : Bill Snook
// Version     : 0.1
// Created on  : Nov 20, 2014
// Copyright   :
// Description : Base for MotionKit
//============================================================================

#include "mkInterface.hpp"

#include "mraa.hpp"



mraa::Gpio* gpio;

mraa::Aio* a0;


mkInterface::mkInterface() {

    response = 0;	// Initialize to no error
    on = 0;			// Default to not on

    iopin = 13;		// Default to onboard LED

    adc_value = 0;
}


mkInterface::~mkInterface() {

    if ( gpio )
    	delete gpio;

    mraa_deinit();
    fprintf(stdout, "\nGoodbye mraa\n\n" );

}


bool mkInterface::setupGPIO( int pinNumber ) {

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
    if (response != MRAA_SUCCESS) {
        mraa_result_print((mraa_result_t) response);
    }
    on = ( 0 == on ) ? 1 : 0;
}


void mkInterface::outputPin( int offOn ) {

    response = gpio->write( offOn );
    if (response != MRAA_SUCCESS) {
        mraa_result_print((mraa_result_t) response);
    }
}


bool mkInterface::setupADC( int pinNumber ) {

	//	adc input setup
	a0 = new mraa::Aio( pinNumber );
	if (!a0) {
	    return MRAA_ERROR_UNSPECIFIED;
	}

	return MRAA_SUCCESS;
}


int mkInterface::readPin() {

	//	adc read
	adc_value = a0->read();

	return adc_value;
}

// end of mkInterface.cpp
