//
// nwInterface.c
//
//  Created on: Nov 30, 2014
//      Author: Bill
//


#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions

#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

#include "nwInterface.h"
#include "nwTime.h"
#include "../commands/parser.h"

#ifndef	DISABLE_IO

int response;
int iopin;
int on;
int isrState;

int adc_value;

double lastTime = 0.0;


//extern	mraa_gpio_context gpio;
//extern	mraa_gpio_context isro;
//extern	mraa_pwm_context pwmo;
//extern	mraa_aio_context aio;


int  serialFDOut;					// file descriptor for serial out
int  serialFDIn;					// file descriptor for input
FILE *fPtrOut;						// Pointer to file
FILE *fPtrIn;						// Make a separate File pointer to handle stdin...


//--	----	----	----	Interface routines


void startMRAA( void ) {

//	printf( "\n  I/O is enabled\n" );
	mraa_init();

	printf( "  MRAA library Version: %s\n", mraa_get_version() );

	mraa_platform_t platformType = mraa_get_platform_type();
    printf( "  Platform type: %d\n", platformType );
    char *platformName = mraa_get_platform_name();
    printf( "  Platform name: %s\n", platformName );
}


//--	----	----	----	General purpose input/output setup


mraa_gpio_context setupGPIOOut( int pinNumber ) {

	return setupGPIO( pinNumber, MRAA_GPIO_OUT );
}


mraa_gpio_context setupGPIOIn( int pinNumber ) {

	return setupGPIO( pinNumber, MRAA_GPIO_IN );
}


mraa_gpio_context setupGPIO( int pinNumber, int direction ) {

    on = 0;

    mraa_gpio_context gpio = mraa_gpio_init( pinNumber );
    if ( ! gpio ) {
        printf( "  Failed initing gpio\n" );
        mraa_result_print( MRAA_ERROR_UNSPECIFIED );
    	return 0;
//    } else {
//        printf( "  Inited gpio: %p\n", gpio );
    }

    response = mraa_gpio_dir( gpio, direction );
    if (response != MRAA_SUCCESS) {
//        printf( "  Failed setting gpio pin direction\n" );
        mraa_result_print((mraa_result_t) response);
        return 0;
    }

   	return gpio;
}


void togglePin( mraa_gpio_context gpio ) {

	response = mraa_gpio_write( gpio, on );
    if (response != MRAA_SUCCESS) {
//        printf( "\n  Failed writing to gpio pin: %p\n", gpio );
        mraa_result_print((mraa_result_t) response);
    }
    on = ( 0 == on ) ? 1 : 0;
}


void outputPin(  mraa_gpio_context gpio, int offOn ) {

	response = mraa_gpio_write( gpio, offOn );
    if (response != MRAA_SUCCESS) {
        mraa_result_print((mraa_result_t) response);
    }
}


int inputPin( mraa_gpio_context gpio ) {

	return mraa_gpio_read( gpio );

}


void closeGPIO( mraa_gpio_context gpio ) {

	mraa_gpio_close( gpio );
}


//--	----	----	----	Analog input setup


mraa_aio_context setupAIO( int pinNumber ) {

	mraa_aio_context aio = mraa_aio_init( pinNumber );
    if ( ! aio ) {
        printf( "  Failed initing aio\n" );
        mraa_result_print( MRAA_ERROR_UNSPECIFIED );
    	return 0;
//    } else {
//        printf( "  Inited aio: %p\n", aio );
    }
	return aio;
}


int readAIO(  mraa_aio_context aio ) {

	return mraa_aio_read( aio );
}


void closeAIO(  mraa_aio_context aio ) {

	mraa_aio_close( aio );
}


//--	----	----	----	Pulse width modulator setup


mraa_pwm_context setupPWMO( int pinNumber ) {

    on = 0;

    mraa_pwm_context pwmo = mraa_pwm_init( pinNumber );
    if ( ! pwmo ) {
        printf( "  Failed initing pwmo\n" );
        mraa_result_print( MRAA_ERROR_UNSPECIFIED );
    	return 0;
    }

    response = mraa_pwm_enable( pwmo, 0 );					// Initially off
    if (response != MRAA_SUCCESS) {
//        printf( "  Failed setting pwmo enable to off during setup\n" );
        mraa_result_print((mraa_result_t) response);
        return 0;
    }
    response = mraa_pwm_config_percent( pwmo, 100, 0.25 );	// Startup settings, 0.1 second, 50% duty cycle
    if (response != MRAA_SUCCESS) {
//        printf( "  Failed setting pwmo period and duty cycle\n" );
        mraa_result_print((mraa_result_t) response);
        return 0;
    }
    response = mraa_pwm_enable( pwmo, 1 );					// Now enable it
    if (response != MRAA_SUCCESS) {
//        printf( "  Failed setting pwmo enable to on during setup\n" );
        mraa_result_print((mraa_result_t) response);
        return 0;
    }

//    printf( "  Inited pwmo for pin %d\n", pinNumber );
   	return pwmo;
}


void setDutyCycle( mraa_pwm_context pwmo, float dutyCycle ) {


}


void closePWMO( mraa_pwm_context pwmo ) {

    response = mraa_pwm_enable( pwmo, 0 );					// Set to off
    if (response != MRAA_SUCCESS) {
//        printf( "  Failed setting pwmo enable to off when closing output\n" );
        mraa_result_print((mraa_result_t) response);
    } else {
    	mraa_pwm_close( pwmo );
    }
}


//--	----	----	----	Interrupt service routine setup


mraa_gpio_context setupISRO( int pinNumber, void (*isrHandler)(void *) ) {

	mraa_gpio_context isro = mraa_gpio_init( pinNumber );
    if ( ! isro ) {
        printf( "  Failed initing isro\n" );
        mraa_result_print( MRAA_ERROR_UNSPECIFIED );
    	return 0;
//    } else {
//        printf( "  Inited isro on pin: %d\n", pinNumber );
    }

    response = mraa_gpio_dir( isro, MRAA_GPIO_IN );
    if (response != MRAA_SUCCESS) {
//        printf( "  Failed setting isro pin direction\n" );
        mraa_result_print((mraa_result_t) response);
        return 0;
    }
//    printf( "  Setup isro pin direction to IN\n" );
    mraa_result_t result = mraa_gpio_isr( isro, MRAA_GPIO_EDGE_BOTH, isrHandler, isro);

    if ( result != MRAA_SUCCESS ) {
//        printf( "  Setup isro interrupt service routine failed with result: %d\n", result );
        return 0;
    }
//    printf( "  Setup isro interrupt service routine\n" );

    // Init to test pin level
//    lastTime = getTimeCheck();
//    isr1( NULL );				// Check level

 	return isro;
}


void closeISRO( mraa_gpio_context isro ) {

	mraa_gpio_isr_exit( isro );
}


void isr1( void *arg ) {

//	mraa_gpio_context isro = (mraa_gpio_context)arg;
///	double thisTime = getTimeCheck();
//	double diff = ((double) (thisTime - lastTime) * 2);

//	int readInput =  mraa_gpio_read( isro );

//	printf( "Interrupt1, state: %d, diff: %.2f uSec\n", readInput, diff );
///	lastTime = thisTime;
}


//--	----	----	----


// Serial 1 port setup
void setupSerial1( int baud ) {
/* */
	mraa_uart_context uart1 = mraa_uart_init( 0 );
	char *devPort = mraa_uart_get_dev_path( uart1 ); // "/dev/ttyMFD1"; // mraa_uart_get_dev_path( uart1 );

	printf( "  setupSerial1 at start with dev path: %s\n", devPort );	// sets up uart1, connected to Arduino BB pins 0 & 1

	serialFDOut = -1;                                  // Say that it is not open...
	serialFDIn = -1;
	fPtrOut = NULL;
	fPtrIn = NULL;

	// Try to open File Descriptor
//	char devPort[] = "/dev/ttyMFD1";
	printf( "  Serial port prepare to open %s\n", devPort );

//	int serialFDOut = open( devPort, O_RDWR| O_NONBLOCK | O_NDELAY );
	int serialFDOut = open( devPort, O_RDWR| O_NONBLOCK | O_NOCTTY | O_SYNC );

	// Error check
	if ( serialFDOut < 0 ) {
		printf( "  Error opening %s: %d, %s\n", devPort, errno, strerror( errno ) );
		return;
	}
	printf( "  Serial port %s opened successfully\n", devPort );

    if ( ( fPtrOut = fdopen( serialFDOut, "r+" ) ) == NULL ) {
		printf( "  Error opening file associated with %s: %d, %s\n", devPort, errno, strerror( errno ) );
		close( serialFDOut );
        return;
    }
	printf( "  Serial port access file %s opened successfully\n", devPort );

    // For normal files _pfileIn will simply be _pfileOut likewise for file descriptors
    fPtrIn = fPtrOut;
    serialFDIn = serialFDOut;

    setvbuf( fPtrOut, NULL, _IONBF, BUFSIZ );
    fflush( fPtrOut );

	// Start port config
	struct termios tty;
	memset(&tty, 0, sizeof tty );

	// Error Handling
	if ( tcgetattr ( serialFDOut, &tty ) != 0 ) {
		printf( "  Error from tcgetattr: %d, %s\n", errno, strerror (errno) );
		fclose( fPtrOut );
		close( serialFDOut );
		return;
	}

	speed_t inSpeed  = cfgetispeed( &tty );
	speed_t outSpeed = cfgetispeed( &tty );
	printf( "  Existing port speed, in: %d, out: %d\n", inSpeed, outSpeed );

	// Setting other Port Stuff
	tty.c_cflag     &=  ~PARENB;        // Make 8n1
	tty.c_cflag     &=  ~CSTOPB;
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     &=  ~CRTSCTS;       // no flow control
	tty.c_cflag     |=  CS8 | HUPCL;	// 8 bits, enable lower control lines on close - hang up
	tty.c_cflag     |=  CREAD | CLOCAL; // turn on READ & ignore ctrl lines

	tty.c_lflag     =   0;          	// no signaling chars, no echo, no canonical processing
//	tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG);	// make raw

	tty.c_oflag     =   0;              // no remapping, no delays
//	tty.c_oflag     &=  ~OPOST;         // make raw
	tty.c_iflag     =   0;	// turn off s/w flow ctrl
//	tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);	// turn off s/w flow ctrl

	tty.c_cc[VMIN]      =   0;          // read doesn't block
	tty.c_cc[VTIME]     =   5;          // 0.5 seconds read timeout

	// Set Baud Rate
	if ( 0 != baud ) {
		if ( cfsetspeed (&tty, baud ) != 0) {
			printf( "  Error from cfsetspeed: %d, %s\n", errno, strerror (errno) );
			return;
		}
	}

	// Flush Port, then applies attributes
	if ( tcflush( serialFDOut, TCIFLUSH ) != 0) {
		printf( "  Error from tcflush: %d, %s\n", errno, strerror (errno) );
		return;
	}

	if ( tcsetattr ( serialFDOut, TCSAFLUSH, &tty ) != 0) {
		printf( "  Error from tcsetattr: %d, %s\n", errno, strerror (errno) );
		return;
	}

    // enable input & output transmission
    if ( tcflow(serialFDOut, TCOON | TCION) != 0) {
		printf( "  Error from tcflow: %d, %s\n", errno, strerror (errno) );
		return;
	}

    // purge buffer
    {
        char buf[1024];
        int n;
        do {
            usleep( 5000 );                         // 5ms
            n = read( serialFDOut, buf, sizeof( buf ) );
        } while ( n > 0 );
    }
    fcntl( serialFDOut, F_SETFL, 0 );               // disable blocking
/**/
}


//--	----	----	----


void testRW( char *msg ) {

	printf( "  Start writing\n\n" );

	// WRITE
	int msgLen = strlen( msg );
	int cnt, loop;
	for ( loop = 0; loop < msgLen; loop++ ) {
		cnt = fwrite( &msg[ loop ], 1, 1, fPtrOut );
	    if ( cnt != 1 ) {
	    	printf( "fwrite error returned %d, errno: %d, %s", cnt, errno, strerror( errno ) );
	    }
	}

//	printf( "  Done writing\n\n" );

    fflush( fPtrOut );

	usleep( 1000 );

	//READ
	char buf [256];									// Allocate memory for read buffer
	memset (&buf, '\0', sizeof buf);

	size_t rdCnt = fread( &buf, 1, 256, fPtrIn );	// Read until end of message?, file?
    if ( rdCnt > 0 ) {
    	printf( "  Read: %s\n", buf );
    }
}


//--	----	----	----


void serialWrite( char byte ) {

	int cnt = fwrite( &byte, 1, 1, fPtrOut );
	if ( cnt != 1 ) {
	   printf( "fwrite error returned %d, errno: %d, %s", cnt, errno, strerror( errno ) );
	}
}


//--	----	----	----


void closeSerial1( void ) {

	fclose( fPtrOut );
	close( serialFDOut );
}


//--	----	----	----


// Finally done, exiting
void closeMRAA( void ) {

	mraa_deinit();
}

#endif	// DISABLE_IO

// End of nwInterface.c

