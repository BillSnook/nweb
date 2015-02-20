//
// timeLoop.h
//
//  Created on: Dec 5, 2014
//      Author: Bill
//

#ifndef TIMELOOP_H_
#define TIMELOOP_H_


#ifdef	DISABLE_IO

#define	DEFAULT_LOOP_TIME	500000

#else	// DISABLE_IO

#define	DEFAULT_LOOP_TIME	100000

#endif	// DISABLE_IO


int			timeLoopExitCode;
long int	timeCheck;
int			showMonitor;


void *monitorTimeOps( void *arg );
void sig_handler(int signo);

void monitorServiceHealth( void );
void displayServiceHealth( void );

#endif /* TIMELOOP_H_ */
