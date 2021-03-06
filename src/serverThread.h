//
// serverThread.h
//
//  Created on: Dec 6, 2014
//      Author: Bill
//

#ifndef SERVERTHREAD_H_
#define SERVERTHREAD_H_


#define	VERSION		0
#define	SUB_VERSION	3


#define BUFSIZE		1024
#define ERROR		42
#define LOG			44
#define FORBIDDEN	403
#define NOTFOUND	404


#ifdef __cplusplus
extern "C"  {
#endif


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


int		servLoopExitCode;
int		webLoopExitCode;

char	*rootDirectory;
char	*baseDirectory;
char	*logFileName;
int		port;
int		running;


//--	----	----	----	----	----	----	----

void nlog(int type, char const *s1, char const *s2, int socket_fd);

void printWebHelp();

int webDirectoryCheck( char *baseDir );

void doParseWebURI( int socketfd, char *commandString );
void *webService( void *arg );
void *monitorWebOps( void *arg );


#ifdef __cplusplus
}
#endif


#endif /* SERVERTHREAD_H_ */
