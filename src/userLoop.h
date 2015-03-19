//
// userLoop.h
//
//  Created on: Dec 5, 2014
//      Author: Bill
//

#ifndef USERLOOP_H_
#define USERLOOP_H_


#define	COMMAND_SZ		256


#ifdef __cplusplus
extern "C"  {
#endif


char	command[COMMAND_SZ];		// storage for command input string

int		commandCode;				// Code for individual commands
int		userLoopExitCode;


void *monitorUserOps( void *arg );

#ifdef __cplusplus
}
#endif


#endif /* USERLOOP_H_ */
