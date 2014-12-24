//
// nwTime.cpp
//
//  Created on: Nov 30, 2014
//      Author: Bill
//


#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>

#include <time.h>

#include "nwTime.h"


double start = 0.0;


void startElapsedTime() {

	start = clock();
//    printf( "\nStart time %ld\n", start );

}


double getElapsedTime() {

	double end = clock();
//	printf( "End time %ld\nElapsed time %f\n", end, ( ((double) (end - start)) / CLOCKS_PER_SEC ));
	return( ((double) (end - start)) / CLOCKS_PER_SEC );
}


double getTimeCheck() {

	return( clock() );
}

// End of nwTime.c
