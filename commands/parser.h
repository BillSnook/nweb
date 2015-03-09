//
// parser.h
//
//  Created on: Dec 6, 2014
//      Author: Bill
//

#ifndef PARSER_H_
#define PARSER_H_


#define	SH_NORMAL_EXIT					0
#define	SH_RUNNING						1
#define	SH_ERROR_BAD_DIRECTORY			-11
#define	SH_ERROR_BAD_PORT				-12
#define	SH_ERROR_INVALID_DIRECTORY		-13
#define	SH_ERROR_PTHREAD_CREATE_ATTR	-21
#define	SH_ERROR_PTHREAD_CREATE_USER	-22
#define	SH_ERROR_PTHREAD_CREATE_TIME	-23
#define	SH_ERROR_PTHREAD_CREATE_WEB		-24
#define	SH_ERROR_PTHREAD_DESTROY_ATTR	-25
#define	SH_ERROR_SOCKET_CREATE			-31
#define	SH_ERROR_SOCKET_BIND			-32
#define	SH_ERROR_SOCKET_LISTEN			-33
#define	SH_ERROR_SOCKET_ATTR			-34
#define	SH_ERROR_SOCKET_ACCEPT			-35
#define	SH_ERROR_PTHREAD_CREATE			-41
#define	SH_ERROR_WEBDATA_MEMORY			-42
#define	SH_ERROR_WEB_READ				-51
#define	SH_ERROR_WEB_PARSE				-52
#define	SH_ERROR_WEB_INVALID_DIRECTORY	-53
#define	SH_ERROR_COMMAND_MEMORY			-61


void putCommand( char type, char value );
char *parseUserCommand( char *command );
char *parseWebCommand( char *command );


#endif /* PARSER_H_ */
