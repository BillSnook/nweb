//
// nwTime.cpp
//
//  Created on: Nov 30, 2014
//      Author: Bill
//


//#include <signal.h>
#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>

#include <time.h>

#include "nwTime.h"


#define	CLOCK_RATE		500000

double start = 0.0;


void startElapsedTime() {

	start = clock();
//    fprintf(stdout, "\nStart time %ld\n", start);

}

double getElapsedTime() {

	double end = clock();
//	fprintf(stdout, "End time %ld\nElapsed time %f\n", end, ( ((double) (end - start)) / CLOCKS_PER_SEC ));
	return( ((double) (end - start)) / CLOCKS_PER_SEC );
}

// End of nwTime.cpp
