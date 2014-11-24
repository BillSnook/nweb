//============================================================================
// Name        : mkInterface.hpp
// Author      : Bill Snook
// Version     : 0.1
// Created on  : Nov 20, 2014
// Copyright   :
// Description : Base for MotionKit
//============================================================================

#ifndef MKINTERFACE_HPP_
#define MKINTERFACE_HPP_


class mkInterface {

public:
	mkInterface();
	virtual ~mkInterface();

	bool setupMRAA( int pinNumber );
	void togglePin();

private:
    int response;
	int iopin;
    int on;

};


#endif /* MKINTERFACE_HPP_ */
