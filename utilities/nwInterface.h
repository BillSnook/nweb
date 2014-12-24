//
// nwInterface.h
//
//  Created on: Nov 30, 2014
//      Author: Bill
//


#ifndef NWINTERFACE_H_
#define NWINTERFACE_H_


#ifdef	ENABLE_IO

#include "mraa.h"


mraa_gpio_context gpio;
mraa_gpio_context isro;
mraa_pwm_context pwmo;


void startMRAA( void );

// General purpose I/O operations
mraa_gpio_context setupGPIO( int pinNumber );

void togglePin( mraa_gpio_context gpio);
void outputPin( mraa_gpio_context gpio, int offOn );

void closeGPIO(  mraa_gpio_context gpio );


// Pulse Width Modulated pin control
mraa_pwm_context setupPWMO( int pinNumber );

void setDutyCycle( mraa_pwm_context pwmo, float dutyCycle );

void closePWMO(  mraa_pwm_context pwmo );


// Interrupt Service trigger pin control
mraa_gpio_context setupISRO( int pinNumber );

void closeISRO(  mraa_gpio_context isro );

void isr1( void *arg );

void closeMRAA( void );


#endif	// ENABLE_IO


#endif /* NWINTERFACE_H_ */
