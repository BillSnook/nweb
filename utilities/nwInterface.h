//
// nwInterface.h
//
//  Created on: Nov 30, 2014
//      Author: Bill
//


#ifndef NWINTERFACE_H_
#define NWINTERFACE_H_


#ifndef	DISABLE_IO

#include "mraa.h"


mraa_gpio_context	gpio;
mraa_gpio_context	isro;
mraa_pwm_context	pwmo;
mraa_aio_context	vDet;
mraa_gpio_context	iSense;


void startMRAA( void );

//--	----	----	----

// General purpose I/O operations
mraa_gpio_context setupGPIOOut( int pinNumber );
mraa_gpio_context setupGPIOIn( int pinNumber );
mraa_gpio_context setupGPIO( int pinNumber, int direction );

void togglePin( mraa_gpio_context gpio);
void outputPin( mraa_gpio_context gpio, int offOn );
int inputPin( mraa_gpio_context gpio );

void closeGPIO(  mraa_gpio_context gpio );

//--	----	----	----

// General purpose analog input processes
mraa_aio_context setupAIO( int pinNumber );

int readAIO(  mraa_aio_context aio );

void closeAIO(  mraa_aio_context aio );

//--	----	----	----

// Pulse Width Modulated pin control
mraa_pwm_context setupPWMO( int pinNumber );

void setDutyCycle( mraa_pwm_context pwmo, float dutyCycle );

void closePWMO(  mraa_pwm_context pwmo );

//--	----	----	----

// Interrupt Service trigger pin control
mraa_gpio_context setupISRO( int pinNumber );

void closeISRO(  mraa_gpio_context isro );

void isr1( void *arg );

//--	----	----	----

void closeMRAA( void );


#endif	// DISABLE_IO


#endif /* NWINTERFACE_H_ */
