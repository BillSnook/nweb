//
// serverThread.h
//
//  Created on: Dec 6, 2014
//      Author: Bill
//

#ifndef SERVERTHREAD_H_
#define SERVERTHREAD_H_


#define	VERSION		0
#define	SUB_VERSION	20


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
	int			socketfd;
	char		*baseDirectory;
};
typedef	struct web_data		web_data;


struct server_data {
	int			port;
	char		*baseDirectory;
};
typedef	struct server_data		server_data;


char	*baseDirectory;
int		port;


//--	----	----	----	----	----	----	----

void nlog(int type, char *s1, char *s2, int socket_fd);

void printWebHelp();

int webDirectoryCheck( char *baseDir );

void doParseWebURI( int socketfd, char *commandString );
void *webService( void *arg );
void *monitorWebOps( void *arg );


#endif /* SERVERTHREAD_H_ */
