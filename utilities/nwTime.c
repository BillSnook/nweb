//
// nwTime.cpp
//
//  Created on: Nov 30, 2014
//      Author: Bill
//


#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>

//#include <time.h>
#include <sys/time.h>

#include "nwTime.h"


long int start = 0;

long int xclock() {

    struct timeval localTime;
    gettimeofday( &localTime, NULL );	// Time zone struct is obsolete, hence NULL

//    printf("The current time in microseconds %ld\n", localTime.tv_usec);

    return( localTime.tv_usec );
}


void startElapsedTime() {

	start = xclock();
//    printf( "\nStart time %ld\n", start );

}


long int getElapsedTime() {

	long int end = xclock();
//	printf( "End time %02f\n", end );
//	printf( "Elapsed time %f\n", ( ((double) (end - start)) / CLOCKS_PER_SEC ));
	end -= start;
	if ( end < 0 )
		end += 1000000;

	return( end );
}


long int getTimeCheck() {

	return( xclock() );
}

// End of nwTime.c
