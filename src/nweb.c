//
//  main.c
//  nweb
//
//  Created by Bill Snook on 11/27/14.
//  Copyright (c) 2014 Bill Snook. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
//#include <semaphore.h>


// This uses fork to create a new untethered process, then halts and returns to the command line
// Otherwise if disabled, the program continues and we get printf output for debugging
// So, off for debugging, on to run as daemon in background
//#define	BECOME_ZOMBIE


// This enables our new html control protocols (vs. html file display method)
#define	NEW_CONTROLS


#include "userLoop.h"
#include "timeLoop.h"


#define BUFSIZE		8096
#define ERROR		42
#define LOG			44
#define FORBIDDEN	403
#define NOTFOUND	404


struct {
	char *ext;
	char *filetype;
} extensions [] = {
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

struct web_data {
	int			listener;
	int			sender;
	int			hit;
};


//--	----	----	----	----	----	----	----


char	*baseDirectory;

static char *html_header = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
static char *html_head = "<html><head>\r\n<title>Edison Web Server</title>\r\n</head><body>\r\n";
static char *html_foot = "\r\n</body></html\r\n>";


//--	----	----	----	----	----	----	----


void nlog(int type, char *s1, char *s2, int socket_fd) {

	int fd ;
	char logbuffer[BUFSIZE*2];

	switch (type) {
		case ERROR:
			(void)sprintf(logbuffer,"ERROR: %s:%s Errno=%d (%d) exiting pid=%d", s1, s2, errno, socket_fd, getpid() );
			break;

//		case SORRY:
//			(void)sprintf( logbuffer, "<HTML><BODY><H1>nweb Web Server Sorry: %s %s</H1></BODY></HTML>\r\n", s1, s2 );
//			(void)write( socket_fd, logbuffer, strlen( logbuffer ) );
//			(void)sprintf( logbuffer,"SORRY: %s:%s",s1, s2 );
//			break;

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


// this is a web server thread, so we can log and pthread_exit on errors
void *web( void *arg ) {
	long i, j, ret;
	char buffer[BUFSIZE+1];
	extern char command[];	// Access commands entered from the command line

	fprintf(stdout, " got html request to handle\n" );

//	pthread_detach( pthread_self() );

	struct web_data *webData = arg;				// extract web params to local struct

	ret = read( webData->sender, buffer, BUFSIZE );  // read Web request in one go

	if ( ret == 0 || ret == -1 ) {     			// read failure stop now
		nlog( FORBIDDEN, "failed to read browser request","", webData->sender );
	}

	if ( ( ret > 0 ) && ( ret < BUFSIZE ) )		// return code is valid chars
		buffer[ret] = 0;          				// terminate the buffer - make it a valid c-string
	else
		buffer[0] = 0;

//	(void)printf( " before cr/lf scrub\n" );
	for ( i = 0; i < ret; i++ )      			// remove CF and LF characters
		if ( ( buffer[i] == '\r' ) || ( buffer[i] == '\n' ) )
			buffer[i] = '*';

	// Kind of cleaned up header received, validate type
	if ( strncmp( buffer, "GET ", 4 ) && strncmp( buffer, "get ", 4 ) ) {	// Verify GET operation
		nlog( FORBIDDEN, "Only simple GET operation supported", buffer, webData->sender );
	}

	// extract uri by terminating string
	for ( i = 4; i < BUFSIZE; i++ ) {			// null terminate after the second space to ignore extra stuff
		if ( buffer[i] == ' ' ) {				// string is "GET URL " + lots of other stuff
			buffer[i] = 0;
			break;
		}
	}

	// Command parsed
	(void)printf( " got request for %s\n", &buffer[4] );

	for ( j = 4; j < i-1; j++ )						// check for illegal parent directory use ..
		if ( ( buffer[j] == '.' ) && ( buffer[j+1] == '.' ) ) {
			nlog( FORBIDDEN, "Parent directory (..) path names not supported", buffer, webData->sender );
		}

#ifdef	NEW_CONTROLS

//	This variant extracts a string from the GET message
//	It then tries to validate the string as a command and then to execute it

	if ( !strncmp( &buffer[0], "GET /\0", 6 ) || !strncmp( &buffer[0], "get /\0", 6 ) ) {	// check for missing uri - special case
		(void)strcpy( buffer, "GET macaroon" );		// Set default command
	}

//	(void)sprintf( buffer, "HTTP/1.1 200 OK\r\nServer: nweb/%d.%d\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: %s\r\n\r\n", VERSION, SUB_VERSION, (long)len, fileType ); // Header + a blank line
//	(void)write( webData->sender, buffer, strlen( buffer ) );

	(void)sprintf( buffer, html_header );	// Shorter version, length not needed
	(void)write( webData->sender, buffer, strlen( buffer ) );

	// send response data, hopefully in html format

	int sz = sprintf( buffer, html_head );		// Start page with string with html and head /head tags and opening body tag

	// Here we sprintf the html contents for display
	sz += sprintf( &buffer[sz], "<h1>Edison return data</h1>\r\n<h2>The data would show here: " );
	sz += sprintf( &buffer[sz], command );
	sz += sprintf( &buffer[sz], "</h2>" );

	sz += sprintf( &buffer[sz], html_foot );	// String with ending /body and /html tag, finalize the page
	(void)write( webData->sender, buffer, strlen( buffer ) );

	(void)printf( " done sending, command: %s\n", command );

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
		nlog( FORBIDDEN, "file extension type not supported", buffer, webData->sender );
	}

	// investigate file name, handle it
	sprintf( filePath, "%s/%s", baseDirectory, &buffer[5]);
	(void)printf( " got filePath: %s\n", filePath );

	// validate the filePath string to determine what to return - default is file at URI path
	if ( ( file_fd = open( filePath, O_RDONLY ) ) == -1 ) {		// open the file for reading
		nlog(NOTFOUND, "failed to open file",  filePath, webData->sender );
	}

	// Send response - only one for now
//	nlog( LOG, "SEND", filePath, webData->hit );
	len = (long)lseek( file_fd, (off_t)0, SEEK_END );			// lseek to the file end to find the length
	(void)lseek(file_fd, (off_t)0, SEEK_SET  );					// lseek back to the file start ready for reading
    (void)sprintf( buffer, "HTTP/1.1 200 OK\r\nServer: nweb/%d.%d\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: %s\r\n\r\n", VERSION, SUB_VERSION, (long)len, fileType ); // Header + a blank line
//	nlog( LOG, "Header", buffer, webData->hit );
	(void)write( webData->sender, buffer, strlen( buffer ) );

//	(void)sprintf( buffer, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fileType );	// Shorter version, length not needed
//	(void)write( webData->sender, buffer, strlen( buffer ) );

	// send file in 8KB block - last block may be smaller
	while ( (ret = read( file_fd, buffer, BUFSIZE ) ) > 0 ) {
		(void)write( webData->sender, buffer, ret );
	}
	(void)printf( " done sending\n" );

#endif	// NEW_CONTROLS

//	sleep( 1 );

	(void)close( webData->sender );

	pthread_exit( NULL );
}


int main(int argc, char **argv) {
	int i, port, listenfd, socketfd, hit, bound;
	size_t length;
//	char *str;
	static struct sockaddr_in cli_addr;		// static = initialised to zeros
	static struct sockaddr_in serv_addr;	// static = initialised to zeros

	if ( argc < 3 || argc > 3 || !strcmp(argv[1], "-?") ) {
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
		exit( 0 );
	}

	if ( !strncmp(argv[2],"/"   ,2 ) || !strncmp(argv[2],"/etc", 5 ) ||
	   !strncmp(argv[2],"/bin",5 ) || !strncmp(argv[2],"/lib", 5 ) ||
	   !strncmp(argv[2],"/tmp",5 ) || !strncmp(argv[2],"/usr", 5 ) ||
	   !strncmp(argv[2],"/dev",5 ) || !strncmp(argv[2],"/sbin",6) ){
		(void)printf("ERROR: Bad top directory %s, see nweb -?\n", argv[2]);
		exit(3);
	}

	if ( chdir(argv[2]) == -1 ) {
		(void)printf("ERROR: Can't Change to directory %s\n",argv[2]);
		exit(4);
	}

	baseDirectory = argv[2];

#ifdef	BECOME_ZOMBIE
	// Create daemon + unstoppable and no zombie children (= no wait())
	int pid;
	if ((pid = fork()) < 0) {
		nlog( ERROR, "system call", "fork", 0 );
	} else {
		if ( pid > 0 ) {						// parent
			return 0;							// parent returns OK to shell
		}
	}
	// child
	for (i = 0; i < 32; i++ )
		(void)close( i );						// close open files

	(void)setpgrp();							// break away from process group

	(void)signal(SIGCHLD, SIG_IGN);				// ignore child death
	(void)signal(SIGHUP, SIG_IGN);				// ignore terminal hangups

#endif	// BECOME_ZOMBIE

	// we want to start a new thread to monitor our user input processes
	pthread_t pThreadUser;	// this is our thread identifier
	int resultUser = pthread_create( &pThreadUser, NULL, monitorUserOps, "param1" );
	if ( 0 != resultUser ) {
		(void)printf( "\n\npthread_create 1 error. Ack!!\n\n" );
		nlog( ERROR, "system call", "pthread_create 1", 0 );
//		exit( 5 );								// parent returns failure to shell
	}

	// we want to start a new thread to monitor our timed processes - like 'blink'
	pthread_t pThreadTime;	// this is our thread identifier
	int resultTime = pthread_create( &pThreadTime, NULL, monitorTimeOps, "param2" );
	if ( 0 != resultTime ) {
		(void)printf( "\n\npthread_create 2 error. Ack!!\n\n" );
		nlog( ERROR, "system call", "pthread_create 2", 0 );
//		exit( 5 );								// parent returns failure to shell
	}

//	nlog( LOG, "nweb starting", argv[1], getpid() );
	(void)printf( "\n\nStarting server now\n\n" );

	// setup the network socket
	if ( ( listenfd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		nlog( ERROR, "system call", "socket", 0 );

	port = atoi(argv[1]);
	if ( port < 0 || port > 60000 )
		nlog( ERROR, "Invalid port number (try 1->60000)", argv[1], 0 );

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	serv_addr.sin_port = htons( port );

	bound = bind( listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); // permission error
	if ( bound < 0 )
		nlog( ERROR, "system call", "bind", 0 );

	if ( listen( listenfd, 64 ) < 0 )
		nlog( ERROR, "system call", "listen", 0 );

	fprintf(stdout, "\nStarting web server process\n\n" );

	pthread_t pThread;	// this is our thread identifier
	for ( hit = 1; ; hit++ ) {
		length = sizeof( cli_addr );
		if ( ( socketfd = accept( listenfd, (struct sockaddr *)&cli_addr, &length) ) < 0 )
			nlog( ERROR, "system call", "accept", 0 );

		struct web_data webData;
		webData.listener = listenfd;
		webData.sender = socketfd;
		webData.hit = hit;

		int result = pthread_create( &pThread, NULL, web, &webData );
		if ( 0 != result ) {
			nlog( ERROR, "system call", "pthread_create x", 0 );
			exit( 5 );								// parent returns failure to shell
		}
	}
}

// End of nweb
