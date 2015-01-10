//
// parser.h
//
//  Created on: Dec 6, 2014
//      Author: Bill
//

#ifndef PARSER_H_
#define PARSER_H_

#define DC_SEND_HEADER   0x56
#define DC_RECV_HEADER   0x76
#define DC_CMD_DIRA      0x73
#define DC_CMD_DIRB      0x74
#define DC_CMD_DIRC      0x75
#define DC_CMD_DIRD      0x76
#define DC_CMD_PWMA      0x80
#define DC_CMD_PWMB      0x81
#define DC_CMD_PWMC      0x82
#define DC_CMD_PWMD      0x83
#define FW               0xff
#define BW               0x00

struct _IO_FILE *serialPort;

char *parseCommand( char *command );


#endif /* PARSER_H_ */
