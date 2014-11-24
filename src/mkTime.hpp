//============================================================================
// Name        : mkTime.hpp
// Author      : Bill Snook
// Version     : 0.1
// Created on  : Nov 14, 2014
// Copyright   :
// Description : Time management routines for MotionKit
//============================================================================

#pragma once

#include <time.h>

#ifndef MKTIME_HPP_
#define MKTIME_HPP_


//namespace mkit {

class mkTime {

public:
	mkTime();
	virtual ~mkTime();

	void startElapsedTime();
	double getElapsedTime();

private:
	clock_t start;
	clock_t end;
};

//} /* namespace mkit */


#endif /* MKTIME_HPP_ */
