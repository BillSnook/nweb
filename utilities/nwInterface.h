//
// nwInterface.h
//
//  Created on: Nov 30, 2014
//      Author: Bill
//

#include <stdio.h>


#ifndef NWINTERFACE_H_
#define NWINTERFACE_H_


int setupGPIO( int pinNumber );
void togglePin();
void outputPin( int offOn );
int closeGPIO( void );



#endif /* NWINTERFACE_H_ */
