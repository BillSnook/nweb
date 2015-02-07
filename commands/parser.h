//
// parser.h
//
//  Created on: Dec 6, 2014
//      Author: Bill
//

#ifndef PARSER_H_
#define PARSER_H_


void putCommand( char type, char value );
char *parseUserCommand( char *command );
char *parseWebCommand( char *command );


#endif /* PARSER_H_ */
