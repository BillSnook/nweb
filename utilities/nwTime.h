//
// nwTime.hpp
//
//  Created on: Nov 30, 2014
//      Author: Bill
//

#ifndef NWTIME_HPP_
#define NWTIME_HPP_


//#define	CLOCK_RATE		500000		// Clocks per second, ~500000Hz, 1/2 MHz

long int xclock( void );

void startElapsedTime();
long int getElapsedTime();
long int getTimeCheck();


#endif /* NWTIME_HPP_ */
