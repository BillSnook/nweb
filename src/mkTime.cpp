//============================================================================
// Name        : mkTime.cpp
// Author      : Bill Snook
// Version     : 0.1
// Created on  : Nov 14, 2014
// Copyright   :
// Description : Time management for MotionKit
//============================================================================

//#include <signal.h>
#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>

#include "mkTime.hpp"

#define	CLOCK_RATE		500000

//namespace mkit {


mkTime::mkTime() {

	start = 0.0;
	end = 0.0;
}

mkTime::~mkTime() {
	// TODO Auto-generated destructor stub
}

void mkTime::startElapsedTime() {

	start = clock();
//    fprintf(stdout, "\nStart time %ld\n", start);

}

double mkTime::getElapsedTime() {

	end = clock();
//	fprintf(stdout, "End time %ld\nElapsed time %f\n", end, ( ((double) (end - start)) / CLOCKS_PER_SEC ));
	return( ((double) (end - start)) / CLOCKS_PER_SEC );
}


//} /* namespace mkit */
