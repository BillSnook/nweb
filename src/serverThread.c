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


// This enables our new html control protocols on top of html file display methods
#define	NEW_CONTROLS


static char *html_header = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
static char *html_head = "<html><head>\r\n<title>Edison Web Server</title>\r\n</head><body>\r\n";
static char *html_foot = "\r\n</body></html\r\n>";

struct fileMap extensions [] = {
	{"gif", "image/gif" },
	{"jpg", "image/jpeg"},
	{"jpeg","image/jpeg"},
	{"png", "image/png" },
	{"ico", "image/ico" },
	{"js",  "application/x-javascript" },
	{"css", "text/css" },
	{"zip", "image/zip" },
	{"gz",  "image/gz"  },
	{"tar", "image/tar" },
	{"htm", "text/html" },
	{"html","text/html" },
	{"txt","text/html" },
	{0,0}
};


//--	----	----	----	----	----	----	----


void nlog(int type, char *s1, char *s2, int socket_fd) {

	int fd ;
	char logbuffer[BUFSIZE*2];

	switch (type) {
		case ERROR:
			sprintf(logbuffer,"ERROR: %s:%s Errno=%d (%d) exiting pid=%d", s1, s2, errno, socket_fd, getpid() );
			break;

		case FORBIDDEN:
			write(socket_fd, "HTTP/1.1 403 Forbidden\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>403 Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n",271);
			sprintf(logbuffer,"FORBIDDEN: %s:%s",s1, s2);
			break;

		case NOTFOUND:
			write(socket_fd, "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\nThe requested URL was not found on this server.\n</body></html>\n",224);
			sprintf(logbuffer,"NOT FOUND: %s:%s",s1, s2);
			break;

		case LOG:
			sprintf(logbuffer," INFO: %s:%s:%d",s1, s2, socket_fd );
			break;

		default:
			sprintf(logbuffer," Unrecognized: %s:%s:%d",s1, s2, socket_fd );
			break;
	}

	// no checks here, nothing can be done a failure anyway
	if ( ( fd = open( "nweb.log", O_CREAT | O_WRONLY | O_APPEND, 0644 ) ) >= 0 ) {
		write( fd, logbuffer, strlen( logbuffer ) );
		write( fd, "\n",  1 );
		close( fd );
	}

	if ( type == ERROR )
		exit( 3 );
	if ( ( type == FORBIDDEN ) || ( type = NOTFOUND ) ) {
//		pthread_exit( NULL );
	}

}


// Check for top level directories where web users should not go, ever
int webDirectoryCheck( char *baseDir ) {
	if ( !strncmp(baseDir,"/"   ,2 ) || !strncmp(baseDir,"/etc", 5 ) ||
			!strncmp(baseDir,"/bin",5 ) || !strncmp(baseDir,"/lib", 5 ) ||
			!strncmp(baseDir,"/tmp",5 ) || !strncmp(baseDir,"/usr", 5 ) ||
			!strncmp(baseDir,"/dev",5 ) || !strncmp(baseDir,"/sbin",6) ) {
//		printf("ERROR: Bad top directory %s, see nweb -?\n", baseDir);
		return 0;
	}
	return 1;
}


void printWebHelp() {

#ifdef	NEW_CONTROLS

//	printf("hint: nweb Port-Number Top-Directory\n\n"
//				 "  nweb is a small and very safe mini web server\n"
//				 "  Specify a port and a named directory to start.\n"
//				 "  There are no fancy features = safe and secure.\n\n"
//				 "  Example: nweb 8080 /home/root/Code/Test &\n\n" );

#else	// NEW_CONTROLS
	int i;

//	printf("hint: nweb Port-Number Top-Directory\n\n"
//				 "\tnweb is a small and very safe mini web server\n"
//				 "\tnweb only servers out file/web pages with extensions named below\n"
//				 "\t and only from the named directory or its sub-directories.\n"
//				 "\tThere is no fancy features = safe and secure.\n\n"
//				 "\tExample: nweb 8080 /home/root/Code/Test &\n\n"
//				 "\tOnly Supports:");
//	for( i = 0; extensions[i].ext != 0; i++ ) {
//		printf(" %s",extensions[i].ext );
//	}

//	printf("\n\tNot Supported: URLs including \"..\", Java, CGI\n"
//					 "\tNot Supported: directories / /etc /bin /lib /tmp /usr /dev/sbin \n"
//					 "\tNo warranty given or implied\n\tNigel Griffiths nag@uk.ibm.com\n"
					 );
#endif	// NEW_CONTROLS

}


void doParse( int socketfd, char *commandString ) {
	char buffer[256];

	extern char command[];	// Access commands entered from the command line

//	This variant extracts a string from the GET message
//	It then tries to validate the string as a command and then to execute it
//	printf( "  received web command to parse: %s\n", commandString );

	// Here we parse the command
	char *returnData = parseCommand( commandString );
	if ( returnData ) {
		sprintf( buffer, html_header );
		write( socketfd, buffer, strlen( buffer ) );
//		printf( "  parse command returned: %s\n", returnData );

		sprintf( buffer, returnData );
		free( returnData );
	} else {
		// Here we create the response page
//		sprintf( buffer, "HTTP/1.1 200 OK\r\nServer: nweb/%d.%d\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: %s\r\n\r\n", VERSION, SUB_VERSION, (long)len, fileType ); // Header + a blank line
//		write( socketfd, buffer, strlen( buffer ) );

		sprintf( buffer, html_header );
		write( socketfd, buffer, strlen( buffer ) );

		// send response data, hopefully in html format
		int sz = sprintf( buffer, html_head );		// Start with html and head /head tags and opening body tag

		// Here we sprintf the html contents for display
		sz += sprintf( &buffer[sz], "<h1>Edison return data</h1>\r\n" );
		sz += sprintf( &buffer[sz], "<h2>The data would show here: " );
		sz += sprintf( &buffer[sz], command );
		sz += sprintf( &buffer[sz], "</h2>" );

		sz += sprintf( &buffer[sz], html_foot );	// String with ending /body and /html tags, finalize the page
	}
	write( socketfd, buffer, strlen( buffer ) );

	strcpy( command, commandString );

}


// this is a web server thread,
//	spawned when data is received,
//	so we can log and pthread_exit on errors
void *webService( void *arg ) {
	long i, j, ret;
	char buffer[BUFSIZE+1];
	extern char command[];	// Access latest command entered from the command line

	int file_fd, buflen, len;
	char * fileType;

	web_data *webData = arg;					// get pointer to web params to local struct
	int socketfd = webData->socketfd;

	ret = read( socketfd, buffer, BUFSIZE );  // read Web request in one go

	if ( ret == 0 || ret == -1 ) {     			// read failure stop now
//		printf( "\n  socket read failure, exit thread\n" );
		nlog( FORBIDDEN, "failed to read browser request","", socketfd );
		close( socketfd );
		free( webData );
		pthread_exit( NULL );
		return NULL;
	}

	if ( ( ret > 0 ) && ( ret < BUFSIZE ) )		// return code is valid chars
		buffer[ret] = 0;          				// terminate the buffer - make it a valid c-string
	else
		buffer[0] = 0;

	// Kind of cleaned up header received, validate type
	if ( strncmp( buffer, "GET ", 4 ) && strncmp( buffer, "get ", 4 ) ) {	// Verify GET operation
//		printf( "\n  non-GET request received, exit thread\n" );
		nlog( FORBIDDEN, "Only simple GET operation supported", buffer, socketfd );
		close( socketfd );
		free( webData );
		pthread_exit( NULL );
		return NULL;
	}

	// Command parsed
	for ( i = 0; i < ret; i++ )      			// remove CF and LF characters
		if ( ( buffer[i] == '\r' ) || ( buffer[i] == '\n' ) )
			buffer[i] = '*';

	// extract uri by terminating string at first space
	for ( i = 4; i < BUFSIZE; i++ ) {			// null terminate after the second space to ignore extra stuff
		if ( buffer[i] == ' ' ) {				// string is "GET URL " + lots of other stuff
			buffer[i] = 0;
			break;
		}
	}

//	printf( "  got GET request: %s\n", &buffer[4] );

	for ( j = 4; j < i-1; j++ )					// check for illegal parent directory use ..
		if ( ( buffer[j] == '.' ) && ( buffer[j+1] == '.' ) ) {
//			printf( "\n  invalid .. directory entry in request, exit thread\n" );
			nlog( FORBIDDEN, "Parent directory (..) path names not supported", buffer, socketfd );
			close( socketfd );
			free( webData );
			pthread_exit( NULL );
			return NULL;
		}

//	This variant extracts the uri from the GET message

	if ( !strncmp( &buffer[0], "GET /\0", 6 ) || !strncmp( &buffer[0], "get /\0", 6 ) ) {	// check for missing uri
//		strcpy( buffer, "GET /moosetrap" );										// default to default null command??
		strcpy( buffer, "GET /index.html" );										// default to index file??
	}

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
	if ( fileType != 0 ) {				// Found extent type that we support
		// investigate file name, handle it
		char filePath[256];
		sprintf( filePath, "%s/%s", webData->baseDirectory, &buffer[5]);
//		printf( "  got filePath: %s\n", filePath );

		// validate the filePath string to determine what to return - default is file at URI path
		if ( ( file_fd = open( filePath, O_RDONLY ) ) != -1 ) {		// open file and check result
//			printf( "  found file at filePath: %s\n", filePath );
			// Send response - only one for now

			len = (long)lseek( file_fd, (off_t)0, SEEK_END );		// lseek to the file end to find the length
			lseek(file_fd, (off_t)0, SEEK_SET  );					// lseek back to the file start ready for reading
		    sprintf( buffer, "HTTP/1.1 200 OK\r\nServer: nweb/%d.%d\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: %s\r\n\r\n", VERSION, SUB_VERSION, (long)len, fileType ); // 1.1 Header + a blank line

			write( socketfd, buffer, strlen( buffer ) );

//			sprintf( buffer, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fileType );	// 1.0 Shorter version, length not needed
//			write( socketfd, buffer, strlen( buffer ) );

			// send file in 8KB blocks - last block may be smaller
			while ( (ret = read( file_fd, buffer, BUFSIZE ) ) > 0 ) {	// read from file,
				write( socketfd, buffer, ret );			// write to client
			}
//			printf( "  done replying for file URI: %s\n\n", filePath );
		} else {													// open the file for reading
			// Hidden command check here - file with recognized type not found
			doParse( socketfd, &buffer[5] );
//			printf( "  done replying for file as command: %s\n\n", command );
		}
	} else {
		// Hidden command check here - unrecognized file name ending
		doParse( socketfd, &buffer[5] );
//		printf( "  done replying to command: %s\n\n", command );
	}

	close( socketfd );
	free( webData );
	pthread_exit( NULL );
	return NULL;
}


//	End of serverThread.c
