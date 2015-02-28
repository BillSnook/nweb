//
// parser.h
//
//  Created on: Dec 6, 2014
//      Author: Bill
//

#ifndef PARSER_H_
#define PARSER_H_


#define	SH_NORMAL_EXIT	0
#define	SH_RUNNING		1
#define	SH_ERROR1		-1
#define	SH_ERROR2		-2


void putCommand( char type, char value );
char *parseUserCommand( char *command );
char *parseWebCommand( char *command );


#endif /* PARSER_H_ */
