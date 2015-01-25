//
// parser.c
//
//  Created on: Dec 6, 2014
//      Author: Bill
//


#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <sys/ioctl.h>  // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions


#include "parser.h"

#include "../src/timeLoop.h"
#include "../utilities/nwInterface.h"

#ifndef	DISABLE_IO
extern	mraa_gpio_context gpio;
extern	mraa_aio_context	vDet;
extern	mraa_gpio_context	iSense;
#endif	// DISABLE_IO

extern int				running;
//extern struct _IO_FILE *serialPort;



//--	----	----	----


char *getADCData( void ) {

#ifdef	DISABLE_IO
	return NULL;
#else	// DISABLE_IO
	togglePin( gpio );
	return NULL;
#endif	// DISABLE_IO
}


//--	----	----	----


void putCommand( char type, char value ) {

/*
    serialWrite( DC_SEND_HEADER );
    serialWrite( type );
    serialWrite( value );
	usleep( 20000 );		// ? needed ?
*/

//	printf( "Output type: 0x%02X, value: 0x%02X\n", type & 0x0FF, value & 0x0FF );
}

//--	----	----	----


char *parseCommand( char *command ) {

//	printf( "  parseCommand %s\n", command );

	if ( 0 == strcmp( "toggle", command ) ) {
		printf( "\n  Got toggle !!\n" );
#ifdef	DISABLE_IO
		return NULL;
#else	// DISABLE_IO
//		togglePin( gpio );
		return NULL;
#endif	// DISABLE_IO
	}

	if ( 0 == strcmp( "exit", command ) ) {			// If command to terminate this program is entered
//		printf( "\n\n  Got exit !!\n\n" );
///		pthread_exit( NULL );						// Kill thread
///		exit( 1 );									// Kill program
		running = 0;
		return NULL;
	}

	if ( 0 == strcmp( "st0", command ) ) {			// Serial Test command
		printf( "  Got st0\n" );
		setupSer( "MFD0", B9600 );
		return NULL;
	}

	if ( 0 == strcmp( "st1", command ) ) {			// Serial Test command
		printf( "  Got st1\n" );
		setupSer( "MFD1", B9600 );
		return NULL;
	}

	if ( 0 == strcmp( "st2", command ) ) {			// Serial Test command
		printf( "  Got st2\n" );
		setupSer( "MFD2", 9600 );
		return NULL;
	}

	if ( 0 == strcmp( "st3", command ) ) {			// Serial Test command
		printf( "  Got st3\n" );
		setupSer( "S0", B9600 );
		return NULL;
	}

	if ( 0 == strcmp( "st4", command ) ) {			// Serial Test command
		printf( "  Got st4\n" );
		setupSer( "S1", B9600 );
		return NULL;
	}

	if ( 0 == strcmp( "st5", command ) ) {			// Serial Test command
		printf( "  Got st5\n" );
		setupSer( "S2", B9600 );
		return NULL;
	}

	if ( 0 == strcmp( "adcData1", command ) ) {
//		printf( "  Got adcData1\n" );
		return getADCData();
	}

	char *buffer = malloc( 1000 );
	if ( 0 == strcmp( "vDet", command ) ) {
//		printf( "  Got vDet, read ADC for vDet on pin 1\n" );
		int adc1 = readAIO( vDet );
		sprintf( buffer, "%d", adc1 & 0x03FF );
		return buffer;
	}

	if ( 0 == strcmp( "iSense", command ) ) {
//		printf( "  Got iSense, detect signal for iSense on pin 9\n" );
		int pin9 = inputPin( iSense );
		sprintf( buffer, "%d", pin9 );
		return buffer;
	}

	int sz = 0; // = sprintf( buffer, html_head );	// Start with html and head /head tags and opening body tag
	if ( 0 == strcmp( "sample1", command ) ) {

//		printf( "  %s\n", command );
		// Here we sprintf the html contents for display
		sz += sprintf( &buffer[sz], "Data from web parser" );

//		sz += sprintf( &buffer[sz], html_foot );	// String with ending /body and /html tags, finalize the page
		return buffer;
	} else if ( 0 == strcmp( "tDirA", command ) ) {
		putCommand( DC_CMD_DIRA, FW);
	} else if ( 0 == strcmp( "tDirB", command ) ) {
		putCommand( DC_CMD_DIRB, FW);
	} else if ( 0 == strcmp( "hiA", command ) ) {
		putCommand( DC_CMD_PWMA, 240);
	} else if ( 0 == strcmp( "hiB", command ) ) {
		putCommand( DC_CMD_PWMB, 240);
	} else if ( 0 == strcmp( "loA", command ) ) {
		putCommand( DC_CMD_PWMA, 20);
	} else if ( 0 == strcmp( "loB", command ) ) {
		putCommand( DC_CMD_PWMB, 20);
	} else {
		return NULL;
	}
	sz += sprintf( &buffer[sz], command );
	return buffer;
}


int  serialFDOut;					// file descriptor for serial out
int  serialFDIn;					// file descriptor for input
FILE *fPtrOut;						// Pointer to file
FILE *fPtrIn;						// Make a separate File pointer to handle stdin...
int  peek;							// last value we received from read/peek


void setupSer( char *portNum02, int baud ) {

	printf( "  setupSer at start\n" );

	serialFDOut = -1;                                  // Say that it is not open...
	serialFDIn = -1;
	fPtrOut = NULL;
	fPtrIn = NULL;
    peek = -1;

	// Try to open File Descriptor
	char devPort[] = "/dev/tty12345678";
	sprintf( devPort, "/dev/tty%s", portNum02 );
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
	if ( cfsetspeed (&tty, baud ) != 0) {
		printf( "  Error from cfsetspeed: %d, %s\n", errno, strerror (errno) );
	}

	// Flush Port, then applies attributes
	if ( tcflush( serialFDOut, TCIFLUSH ) != 0) {
		printf( "  Error from tcflush: %d, %s\n", errno, strerror (errno) );
	}

	if ( tcsetattr ( serialFDOut, TCSAFLUSH, &tty ) != 0) {
		printf( "  Error from tcsetattr: %d, %s\n", errno, strerror (errno) );
	}

    // enable input & output transmission
    if ( tcflow(serialFDOut, TCOON | TCION) != 0) {
		printf( "  Error from tcflow: %d, %s\n", errno, strerror (errno) );
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

	// WRITE

	printf( "  Start\n\n" );

    int i;
    for ( i = 0; i < 10; i++ ) {
        serWrite( '?' );		// 0x3F
    }

	printf( "  Done\n\n" );

/*
    serWrite( 'I' );
    serWrite( 'N' );
    serWrite( 'I' );
    serWrite( 'T' );
    serWrite( '\n' );
    serWrite( '\0' );
    //
	unsigned char cmd[] = {'I', 'N', 'I', 'T', ' ', '\r', '\0'};
	int n_written = write( serialFDOut, cmd, sizeof(cmd) - 1 );

	if ( n_written < ( sizeof(cmd) - 1 ) ) {
		printf( "  Error writing, intended: %d, wrote: %d\n", sizeof(cmd) - 1, n_written );
	}
*/
/*
	// Allocate memory for read buffer
	char buf [256];
	memset (&buf, '\0', sizeof buf);

	//READ
	int n_read = read( serialFDOut, &buf, sizeof buf );

	// Error Handling
	if ( n_read < 0 ) {
		printf( "  Error reading: %d, %s\n", errno, strerror (errno) );
	}

	// Print what I read...
	printf( "  Read: %s\n", buf );
*/
	int cnt = serPeek();
	printf( "  Peek: %d\n", cnt );

	fclose( fPtrOut );
	close( serialFDOut );
}


int serAvailable(void) {
    int bytes;

    // BUGBUG:: handle standard terminal better
    ioctl( serialFDIn, FIONREAD, &bytes );
    return bytes;
}


int serPeek( void ) {
    if ( peek == -1 )
        peek = serRead();

    return peek;
}


int serRead( void ) {
    if ( peek != -1 ) {
        int iRet = peek;
        peek = -1;
        return iRet;
    }

    unsigned char b;
    if ( fread( &b, 1, 1, fPtrIn ) )
        return b;
    return -1;
}

void serFlush( void ) {
    fflush( fPtrOut );
}


size_t serWrite( unsigned char c ) {
	int cnt;
    if ( ( cnt = fwrite( &c, 1, 1, fPtrOut ) ) != 1 ) {
    	printf( "fwrite error returned %d", cnt );
    }
    return 1;
}


// End of parser.c
