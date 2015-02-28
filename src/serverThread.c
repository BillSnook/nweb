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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <stdarg.h>

//#include "userLoop.h"
//#include "timeLoop.h"
#include "serverThread.h"
#include "../commands/parser.h"


// This enables our new html control protocols on top of html file display methods
#define	NEW_CONTROLS

// Basic simple HTTP/v1.0 header
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

extern int		running;
extern char		*baseDirectory;
//extern char		*logFileName;
extern int		port;
extern int		servLoopExitCode;
extern int		webLoopExitCode;


int				runningWebLoop;

pthread_mutex_t	parseWebMutex = PTHREAD_MUTEX_INITIALIZER;


//--	----	----	----	----	----	----	----

void blog( const char *__restrict __format, ... ) {

	va_list arglist;
	va_start( arglist, __format );
	vfprintf( stdout,  __format, arglist );
//	vfprintf( stderr,  __format, arglist );
	va_end( arglist );

}

void nlog(int type, char *s1, char *s2, int errorCodeOrSkt) {

	int fd ;
	char logbuffer[BUFSIZE*2];

	switch (type) {
		case ERROR:
			sprintf(logbuffer,"ERROR: %s %s Errno=%d (%s), Err=%d, pid=%d", s1, s2, errno, strerror(errno), errorCodeOrSkt, getpid() );
			printf( "ERROR: %s %s Errno=%d (%s), Err=%d, pid=%d", s1, s2, errno, strerror(errno), errorCodeOrSkt, getpid() );
			break;

		case FORBIDDEN:
			write(errorCodeOrSkt, "HTTP/1.1 403 Forbidden\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>403 Forbidden</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n",271);
			sprintf(logbuffer,"FORBIDDEN: %s:%s",s1, s2);
			break;

		case NOTFOUND:
			write(errorCodeOrSkt, "HTTP/1.1 404 Not Found\nContent-Length: 136\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Not Found</h1>\nThe requested URL was not found on this server.\n</body></html>\n",224);
			sprintf(logbuffer,"NOT FOUND: %s:%s",s1, s2);
			break;

		case LOG:
			sprintf(logbuffer," INFO: %s %s: %d",s1, s2, errorCodeOrSkt );
			break;

		default:
			sprintf(logbuffer," Unrecognized: %s %s: %d",s1, s2, errorCodeOrSkt );
			break;
	}

	char logFN[256];
	sprintf( logFN, "%s/nweb.log", rootDirectory );

	if ( ( fd = open( logFN, O_CREAT | O_WRONLY | O_APPEND, 0644 ) ) >= 0 ) {
		write( fd, logbuffer, strlen( logbuffer ) );
		write( fd, "\n",  1 );
		close( fd );
	}

	if ( type == ERROR ) {
		running = 0;
		sleep( 2 );
//		pthread_exit( NULL );
//		exit( 70 );
	}

//	if ( ( type == FORBIDDEN ) || ( type = NOTFOUND ) ) {
//		pthread_exit( NULL );
//	}

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


// this applies to web GET command line strings
void doParseWebURI( int socketfd, char *commandString ) {
	char buffer[256];

	extern char command[];	// Access commands entered from the command line

//	This variant extracts a string from the GET message
//	It then tries to validate the string as a command and then to execute it
//	printf( "  received web command to parse: %s\n", commandString );

	// Here we parse the command
	char *returnData = parseWebCommand( commandString );
	// for now we are confused
	// if data is returned we want to return it to the web client
	// but if there is no return data, we want to signal that too
	if ( returnData ) {						// this call resulted in data to be returned as html
		sprintf( buffer, html_header );		// signal legitimate html data returned - not CGI
											// it's just not a full html file since this is usually ajax
		write( socketfd, buffer, strlen( buffer ) );
//		printf( "  parse command returned: %s\n", returnData );

		sprintf( buffer, returnData );
		free( returnData );
		// continue to buffer write sequence
	} else {								// this call resulted in no data to be returned as html
		// Here we create the response page
//		sprintf( buffer, "HTTP/1.1 200 OK\r\nServer: nweb/%d.%d\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: %s\r\n\r\n", VERSION, SUB_VERSION, (long)len, fileType ); // Header + a blank line
//		write( socketfd, buffer, strlen( buffer ) );

		sprintf( buffer, html_header );
		write( socketfd, buffer, strlen( buffer ) );

		// send response data, hopefully in html format
		int sz = sprintf( buffer, html_head );		// Start with html and head /head tags and opening body tag

		// Here we sprintf the html contents for display
		// for now just display the current command
		sz += sprintf( &buffer[sz], "<h1>Edison did not return data</h1>\r\n" );
		sz += sprintf( &buffer[sz], "<h2>The current command is here: " );
		sz += sprintf( &buffer[sz], command );
		sz += sprintf( &buffer[sz], "</h2>" );

		sz += sprintf( &buffer[sz], html_foot );	// String with ending /body and /html tags, finalize the page
		// continue to buffer write sequence
	}
	write( socketfd, buffer, strlen( buffer ) );

	strcpy( command, commandString );				// save for the record

}


// this is a web server thread,
//	spawned when data is received,
//	so we can log and pthread_exit on errors
void *webService( void *arg ) {
	long i, j, ret;
	char buffer[BUFSIZE+1];
	extern char command[];	// Access latest command entered from the command line

	int buflen, len;
	char * fileType;

	if ( ++webLoopExitCode > 99 )
		webLoopExitCode = 0;

	web_data *webData = arg;					// get pointer to web params to local struct
	int socketfd = webData->socketfd;			// receive socket with data

	ret = read( socketfd, buffer, BUFSIZE );	// read Web request in one go

	if ( ret <= 0 ) {     						// if read failure then reply and exit thread
//		printf( "\n  socket read failure, exit thread\n" );
		close( socketfd );
		free( webData );
		webLoopExitCode = 52;
		nlog( FORBIDDEN, "failed to read browser request","", socketfd );
//		pthread_exit( NULL );
		return NULL;
	}

	if ( ( ret > 0 ) && ( ret < BUFSIZE ) )		// return code is valid chars
		buffer[ret] = 0;          				// terminate the buffer - make it a valid c-string
	else
		buffer[0] = 0;							// else default to empty string

	// Kind of cleaned up header received, validate type
	if ( strncmp( buffer, "GET ", 4 ) && strncmp( buffer, "get ", 4 ) ) {	// Verify GET operation
//		printf( "\n  non-GET request received, exit thread\n" );
		close( socketfd );
		free( webData );
		webLoopExitCode = 54;
		nlog( FORBIDDEN, "Only simple GET operation supported", buffer, socketfd );
//		pthread_exit( NULL );
		return NULL;
	}

	// Command parsed, cleanup GET string
	for ( i = 0; i < ret; i++ )      			// remove CF and LF characters
		if ( ( buffer[i] == '\r' ) || ( buffer[i] == '\n' ) )
			buffer[i] = '*';

	// extract uri by terminating string at first space
	for ( i = 4; i < BUFSIZE; i++ ) {			// null terminate after the second space to ignore extra stuff
		if ( buffer[i] == ' ' ) {				// string is "GET URL " + lots of other stuff
			buffer[i] = 0;
			break;
		}
	}		// leave with i set to the length of URI string

//	printf( "  got GET request: %s\n", &buffer[4] );

	for ( j = 4; j < i-1; j++ )					// check for illegal parent directory use ..
		if ( ( buffer[j] == '.' ) && ( buffer[j+1] == '.' ) ) {
//			printf( "\n  invalid .. directory entry in request, exit thread\n" );
			close( socketfd );
			free( webData );
			webLoopExitCode = 56;
			nlog( FORBIDDEN, "Parent directory (..) path names not supported", buffer, socketfd );
//			pthread_exit( NULL );
			return NULL;
		}

//	Get the uri from the GET message

	if ( !strncmp( &buffer[0], "GET /\0", 6 ) || !strncmp( &buffer[0], "get /\0", 6 ) ) {	// check for missing uri
		strcpy( buffer, "GET /index.html" );										// default to index file??
	}

	// extract the file type and check we support it
	buflen = (int)strlen( buffer );
	fileType = (char *)0;

	// compare against list of extensions we support
	for ( i = 0; extensions[i].ext != 0; i++ ) {
		len = (int)strlen( extensions[i].ext );
		if ( !strncmp( &buffer[buflen-len], extensions[i].ext, len ) ) {
			fileType = extensions[i].filetype;
			break;
		}
	}

	if ( fileType != 0 ) {				// Found extent type that we support - if found open, get contents, prepend http header and reply
		int file_fd;
		// investigate file name, open it and send it as a response
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
			close( file_fd );
//			printf( "  done replying for file URI: %s\n\n", filePath );
		} else {													// could not find or open the file - assume it is a command
			// Hidden command check here - file with recognized type not found
            pthread_mutex_lock( &parseWebMutex );
			doParseWebURI( socketfd, &buffer[5] );
            pthread_mutex_unlock( &parseWebMutex );
//			printf( "  done replying for file as command: %s\n\n", command );
		}
	} else {														// could not recognize the file type - assume it is a command
		// Hidden command check here - unrecognized file name ending
        pthread_mutex_lock( &parseWebMutex );
		doParseWebURI( socketfd, &buffer[5] );
        pthread_mutex_unlock( &parseWebMutex );
//		printf( "  done replying to command: %s\n\n", command );
	}

	webLoopExitCode = 50;		// Normal web service request handler exit code

	close( socketfd );
	free( webData );
	pthread_exit( NULL );
	return NULL;
}


// this routine is created as it's own thread and monitors the web server port
//   and spawns a webService thread to handle each request that is received
void *monitorWebOps( void *arg ) {
	int listenfd, requestfd, bound;
	size_t length;
	static struct sockaddr_in reply_socketaddr;		// receive socket address from accept routine
	static struct sockaddr_in listen_socketaddr;	// listen socket socket address

	webLoopExitCode = SH_RUNNING;
	servLoopExitCode = SH_RUNNING;

	// setup the network socket
	if ( ( listenfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
		servLoopExitCode = 41;
		nlog( ERROR, "creating new socket", "failed", 80 );
		return NULL;
	}

	listen_socketaddr.sin_family = AF_INET;
	listen_socketaddr.sin_addr.s_addr = htonl( INADDR_ANY );
	listen_socketaddr.sin_port = htons( port );

	bound = bind( listenfd, (struct sockaddr *)&listen_socketaddr, sizeof(listen_socketaddr)); // permission error
	if ( bound < 0 ) {
		servLoopExitCode = 43;
		nlog( ERROR, "binding to socket", "failed", 81 );
		pthread_exit( NULL );
		return NULL;
	}

	if ( listen( listenfd, 64 ) < 0 ) {
		servLoopExitCode = 44;
		nlog( ERROR, "creating listener socket", "failed", 82 );
		pthread_exit( NULL );
		return NULL;
	}

	blog( "\nStarting web server process, version %d.%d\n", VERSION, SUB_VERSION );
//	printf( "\nStarting web server process, version %d.%d\n", VERSION, SUB_VERSION );

	pthread_t pThread;	// this is our thread identifier
    pthread_attr_t attr;
    int s = pthread_attr_init( &attr );
    if ( s != 0 ) {
		servLoopExitCode = 45;
		nlog( ERROR, "pthread_attr_init", "failed", 83 );
		pthread_exit( NULL );
		return NULL;
	}

    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

	while ( running ) {
		if ( ++servLoopExitCode > 99 )
			servLoopExitCode = SH_RUNNING;

		length = sizeof( reply_socketaddr );
		if ( ( requestfd = accept( listenfd, (struct sockaddr *)&reply_socketaddr, &length) ) < 0 ) {
			servLoopExitCode = 46;
			nlog( ERROR, "accepting request on listener", "failed", 90 );
			pthread_exit( NULL );
			return NULL;
		}

		web_data *webData = malloc( sizeof( web_data ) );
		if ( webData ) {
			webData->socketfd = requestfd;
			webData->baseDirectory = baseDirectory;

			int result = pthread_create( &pThread, &attr, webService, webData );
			if ( 0 != result ) {
				servLoopExitCode = 47;
				nlog( ERROR, "creating a thread to handle new request", "failed", 91 );
				free( webData );
				pthread_exit( NULL );
				return NULL;
			}
		} else {
			servLoopExitCode = 48;
			nlog( ERROR, "allocating web_data struct for request", "failed", 92 );
			pthread_exit( NULL );
			return NULL;
		}
	}
	printf( "\n  Web server loop ended\n" );
	servLoopExitCode = SH_NORMAL_EXIT;
	pthread_attr_destroy( &attr );
	return NULL;
}


//	End of serverThread.c
