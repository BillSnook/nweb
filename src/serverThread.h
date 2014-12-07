//
// serverThread.h
//
//  Created on: Dec 6, 2014
//      Author: Bill
//

#ifndef SERVERTHREAD_H_
#define SERVERTHREAD_H_


#define BUFSIZE		8096
#define ERROR		42
#define LOG			44
#define FORBIDDEN	403
#define NOTFOUND	404


struct fileMap {
	char *ext;
	char *filetype;
} fileMap;


struct web_data {
//	int			listener;
	int			socketfd;
//	int			hit;
};
typedef	struct web_data		web_data;

//--	----	----	----	----	----	----	----

void nlog(int type, char *s1, char *s2, int socket_fd);

void printWebHelp();

void *webService( void *arg );


#endif /* SERVERTHREAD_H_ */
