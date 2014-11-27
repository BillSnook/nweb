//============================================================================
// Name        : mkInterface.hpp
// Author      : Bill Snook
// Version     : 0.1
// Created on  : Nov 20, 2014
// Copyright   :
// Description : Base for MotionKit
//============================================================================

//#ifndef MKINTERFACE_HPP_
//#define MKINTERFACE_HPP_

#include <stdio.h>


class mkInterface {

public:
	mkInterface();
	virtual ~mkInterface();

	bool setupGPIO( int pinNumber );
	void togglePin();
	void outputPin( int offOn );

	bool setupADC( int pinNumber );
	int readPin();		// 0 - 3FFh, 0 - 1023

private:
    int response;
	int iopin;
    int on;

	int adc_value;

};


//#endif /* MKINTERFACE_HPP_ */
