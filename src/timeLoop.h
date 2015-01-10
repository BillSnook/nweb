//
// timeLoop.h
//
//  Created on: Dec 5, 2014
//      Author: Bill
//

#ifndef TIMELOOP_H_
#define TIMELOOP_H_


int		running;
double	timeCheck;


void *monitorTimeOps( void *arg );
void sig_handler(int signo);


#endif /* TIMELOOP_H_ */
