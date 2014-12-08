//
// serverThread.c
//
//  Created on: Dec 6, 2014
//      Author: Bill
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include <pthread.h>

//#include "userLoop.h"
//#include "timeLoop.h"
#include "serverThread.h"
#include "../commands/parser.h"



// This enables our new html control protocols (vs. html file display method)
#define	NEW_CONTROLS


static char *html_header = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
static char *html_head = "<html><head>\r\n<title>Edison Web Server</title>\r\n</head><body>\r\n";
static char *html_foot = "\r\n</body></html\r\n>";

#ifndef	NEW_CONTROLS

struct fileMap extensions [] = {
	{"gif", "image/gif" },
	{"jpg", "image/jpeg"},
	{"jpeg","image/jpeg"},
	{"png", "image/png" },
	{"ico", "image/ico" },
	{"js",  "application/x-javascript" },
	{"zip", "image/zip" },
	{"gz",  "image/gz"  },
	{"tar", "image/tar" },
	{"htm", "text/html" },
	{"html","text/html" },
	{0,0}
};

#endif	// NEW_CONTROLS


//--	----	----	----	----	----	----	----


void nlog(int type, char *s1, char *s2, int socket_fd) {

	int fd ;
	char logbuffer[BUFSIZE*2];

	switch (type) {
		case ERROR:
			(void)sprintf(logbuffer,"ERROR: %s:%s Errno=%d (%d) exiting pid=%d", s1, s2, errno, socket_fd, getpid() );
			break;

		case FORBIDDEN:
			(void)write(socket_fd, "HTTP/1.1 403 Forbidden\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>403 Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n",271);
			(void)sprintf(logbuffer,"FORBIDDEN: %s:%s",s1, s2);
			break;

		case NOTFOUND:
			(void)write(socket_fd, "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\nThe requested URL was not found on this server.\n</body></html>\n",224);
			(void)sprintf(logbuffer,"NOT FOUND: %s:%s",s1, s2);
			break;

		case LOG:
			(void)sprintf(logbuffer," INFO: %s:%s:%d",s1, s2, socket_fd );
			break;

		default:
			(void)sprintf(logbuffer," Unrecognized: %s:%s:%d",s1, s2, socket_fd );
			break;
	}

	// no checks here, nothing can be done a failure anyway
	if ( ( fd = open( "nweb.log", O_CREAT | O_WRONLY | O_APPEND, 0644 ) ) >= 0 ) {
		(void)write( fd, logbuffer, strlen( logbuffer ) );
		(void)write( fd, "\n",  1 );
		(void)close( fd );
	}

	if ( type == ERROR )
		exit( 3 );
	if ( ( type == FORBIDDEN ) || ( type = NOTFOUND ) ) {
//		pthread_exit( NULL );
	}

}


void printWebHelp() {

#ifdef	NEW_CONTROLS

	(void)printf("hint: nweb Port-Number Top-Directory\n\n"
				 "  nweb is a small and very safe mini web server\n"
				 "  Specify a port and a named directory to start.\n"
				 "  There are no fancy features = safe and secure.\n\n"
				 "  Example: nweb 8080 /home/root/Code/Test &\n\n"
				 );

	#else	// NEW_CONTROLS
	int i;

	(void)printf("hint: nweb Port-Number Top-Directory\n\n"
				 "\tnweb is a small and very safe mini web server\n"
				 "\tnweb only servers out file/web pages with extensions named below\n"
				 "\t and only from the named directory or its sub-directories.\n"
				 "\tThere is no fancy features = safe and secure.\n\n"
				 "\tExample: nweb 8080 /home/root/Code/Test &\n\n"
				 "\tOnly Supports:");
	for( i = 0; extensions[i].ext != 0; i++ )
		(void)printf(" %s",extensions[i].ext );

	(void)printf("\n\tNot Supported: URLs including \"..\", Java, Javascript, CGI\n"
					 "\tNot Supported: directories / /etc /bin /lib /tmp /usr /dev/sbin \n"
					 "\tNo warranty given or implied\n\tNigel Griffiths nag@uk.ibm.com\n"
					 );
#endif	// NEW_CONTROLS

}


// this is a web server thread, so we can log and pthread_exit on errors
void *webService( void *arg ) {
	long i, j, ret;
	char buffer[BUFSIZE+1];
	extern char command[];	// Access commands entered from the command line

	fprintf(stdout, "\n" );
//	fprintf(stdout, "  got html request to handle\n" );

//	pthread_detach( pthread_self() );

	web_data *webData = arg;				// get pointer to web params to local struct

	ret = read( webData->socketfd, buffer, BUFSIZE );  // read Web request in one go

	if ( ret == 0 || ret == -1 ) {     			// read failure stop now
		nlog( FORBIDDEN, "failed to read browser request","", webData->socketfd );
	}

	if ( ( ret > 0 ) && ( ret < BUFSIZE ) )		// return code is valid chars
		buffer[ret] = 0;          				// terminate the buffer - make it a valid c-string
	else
		buffer[0] = 0;

	// Kind of cleaned up header received, validate type
	if ( strncmp( buffer, "GET ", 4 ) && strncmp( buffer, "get ", 4 ) ) {	// Verify GET operation
		nlog( FORBIDDEN, "Only simple GET operation supported", buffer, webData->socketfd );
	}

	// Command parsed
	(void)printf( "  got request:\n%s\n", &buffer[4] );

	for ( i = 0; i < ret; i++ )      			// remove CF and LF characters
		if ( ( buffer[i] == '\r' ) || ( buffer[i] == '\n' ) )
			buffer[i] = '*';

	// extract uri by terminating string
	for ( i = 4; i < BUFSIZE; i++ ) {			// null terminate after the second space to ignore extra stuff
		if ( buffer[i] == ' ' ) {				// string is "GET URL " + lots of other stuff
			buffer[i] = 0;
			break;
		}
	}

	for ( j = 4; j < i-1; j++ )						// check for illegal parent directory use ..
		if ( ( buffer[j] == '.' ) && ( buffer[j+1] == '.' ) ) {
			nlog( FORBIDDEN, "Parent directory (..) path names not supported", buffer, webData->socketfd );
		}

#ifdef	NEW_CONTROLS

//	This variant extracts a string from the GET message
//	It then tries to validate the string as a command and then to execute it
	printf( "  received web command:\n%s\n", buffer );

	if ( !strncmp( &buffer[0], "GET /\0", 6 ) || !strncmp( &buffer[0], "get /\0", 6 ) ) {	// check for missing uri - special case
		(void)strcpy( buffer, "GET /macaroon" );		// Set default command
	}

	// Here we parse the command
	char *commandString = &buffer[5];
	parseCommand( commandString );

	// Here we create the response page

//	(void)sprintf( buffer, "HTTP/1.1 200 OK\r\nServer: nweb/%d.%d\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: %s\r\n\r\n", VERSION, SUB_VERSION, (long)len, fileType ); // Header + a blank line
//	(void)write( webData->socketfd, buffer, strlen( buffer ) );

	(void)sprintf( buffer, html_header );	// Shorter version, length not needed
	(void)write( webData->socketfd, buffer, strlen( buffer ) );

	// send response data, hopefully in html format

	int sz = sprintf( buffer, html_head );		// Start page with string with html and head /head tags and opening body tag

	// Here we sprintf the html contents for display
	sz += sprintf( &buffer[sz], "<h1>Edison return data</h1>\r\n" );
	sz += sprintf( &buffer[sz], "<h2>The data would show here: " );
	sz += sprintf( &buffer[sz], command );
	sz += sprintf( &buffer[sz], "</h2>" );

	sz += sprintf( &buffer[sz], html_foot );	// String with ending /body and /html tag, finalize the page
	(void)write( webData->socketfd, buffer, strlen( buffer ) );

	(void)printf( "  done sending, command: %s\n", command );

#else	// NEW_CONTROLS

	int file_fd, buflen, len;
	char * fileType;
	static char filePath[256];					// static so zero filled

//	This variant extracts the uri from the GET message

	if ( !strncmp( &buffer[0], "GET /\0", 6 ) || !strncmp( &buffer[0], "get /\0", 6 ) )	// check for missing uri
		(void)strcpy( buffer, "GET /index.html" );										// default to index file

	// work out the file type and check we support it
	buflen = (int)strlen( buffer );
	fileType = (char *)0;

	for ( i = 0; extensions[i].ext != 0; i++ ) {
		len = (int)strlen( extensions[i].ext );
		if ( !strncmp( &buffer[buflen-len], extensions[i].ext, len ) ) {
			fileType = extensions[i].filetype;
			break;
		}
	}
	if ( fileType == 0 ) {
		nlog( FORBIDDEN, "file extension type not supported", buffer, webData->socketfd );
	}

	// investigate file name, handle it
	sprintf( filePath, "%s/%s", baseDirectory, &buffer[5]);
	(void)printf( " got filePath: %s\n", filePath );

	// validate the filePath string to determine what to return - default is file at URI path
	if ( ( file_fd = open( filePath, O_RDONLY ) ) == -1 ) {		// open the file for reading
		nlog(NOTFOUND, "failed to open file",  filePath, webData->socketfd );
	}

	// Send response - only one for now
//	nlog( LOG, "SEND", filePath, webData->hit );
	len = (long)lseek( file_fd, (off_t)0, SEEK_END );			// lseek to the file end to find the length
	(void)lseek(file_fd, (off_t)0, SEEK_SET  );					// lseek back to the file start ready for reading
    (void)sprintf( buffer, "HTTP/1.1 200 OK\r\nServer: nweb/%d.%d\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: %s\r\n\r\n", VERSION, SUB_VERSION, (long)len, fileType ); // Header + a blank line
//	nlog( LOG, "Header", buffer, webData->hit );
	(void)write( webData->socketfd, buffer, strlen( buffer ) );

//	(void)sprintf( buffer, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fileType );	// Shorter version, length not needed
//	(void)write( webData->socketfd, buffer, strlen( buffer ) );

	// send file in 8KB block - last block may be smaller
	while ( (ret = read( file_fd, buffer, BUFSIZE ) ) > 0 ) {
		(void)write( webData->socketfd, buffer, ret );
	}
	(void)printf( " done sending\n" );

#endif	// NEW_CONTROLS

	(void)close( webData->socketfd );

	pthread_exit( NULL );

	return NULL;
}


//	End of serverThread.c
