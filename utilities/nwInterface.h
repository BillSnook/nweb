//
// nwInterface.h
//
//  Created on: Nov 30, 2014
//      Author: Bill
//


#ifndef NWINTERFACE_H_
#define NWINTERFACE_H_

#include "mraa.h"


mraa_gpio_context gpio;


void startMRAA( void );

mraa_gpio_context setupGPIO( int pinNumber );

void togglePin( mraa_gpio_context gpio);
void outputPin( mraa_gpio_context gpio, int offOn );

void closeGPIO(  mraa_gpio_context gpio );

void closeMRAA( void );



#endif /* NWINTERFACE_H_ */
