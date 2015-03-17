/*
 * motorDriver.hpp
 *
 *  Created on: Mar 8, 2015
 *      Author: bill
 */

#ifndef MOTORDRIVER_HPP_
#define MOTORDRIVER_HPP_

#ifndef	DISABLE_IO


#include "../utilities/nwInterface.h"

#include "mraa.hpp"


namespace device {


void setupDevice( void );


class driver {
    public:
        // Instantiates a driver object
        driver() {

//            startMRAA();	// presumed stated
            iSense = setupGPIOIn( 9 );
            vDet = setupAIO( 0 );

            setupSerial1( 1 );
        }
        // driver object destructor
        ~driver() {

        	closeSerial1();
            closeAIO( vDet );
            closeGPIO( iSense );

        }
    private:
        mraa_aio_context	vDet;
        mraa_gpio_context	iSense;

};


}

#endif // DISABLE_IO

#endif // MOTORDRIVER_HPP_
