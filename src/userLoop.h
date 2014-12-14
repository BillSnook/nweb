//
// userLoop.h
//
//  Created on: Dec 5, 2014
//      Author: Bill
//

#ifndef USERLOOP_H_
#define USERLOOP_H_


#define	COMMAND_SZ		256

char	command[COMMAND_SZ];		// storage for command input string

int		commandCode;				// Code for individual commands


void *monitorUserOps( void *arg );


#endif /* USERLOOP_H_ */
