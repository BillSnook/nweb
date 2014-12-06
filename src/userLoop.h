//
// userLoop.h
//
//  Created on: Dec 5, 2014
//      Author: Bill
//

#ifndef USERLOOP_H_
#define USERLOOP_H_


char	command[256];				// storage for input

int		commandCode;				// Code for individual commands


void *monitorUserOps( void *arg );


#endif /* USERLOOP_H_ */
