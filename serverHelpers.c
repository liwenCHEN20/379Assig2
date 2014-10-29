#include "serverHelpers.h"
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

int is_valid_path(char * file){
	//Makeing sure we have a valid path
	DIR * dir = opendir(file);
	if (dir != NULL){
		closedir(dir);
		return 1;
	}
	usage();
}

int is_valid_port(char * portStr){
	errno = 0;
	char * ep;

	long p = strtoul(portStr, &ep, 10);
    if (*portStr == '\0' || *ep != '\0') {
		/* parameter wasn't a number, or was empty */
		fprintf(stderr, "%s - not a number\n", portStr);
		usage();
	}
        if ((errno == ERANGE && p == ULONG_MAX) || (p > USHRT_MAX)) {
			/* It's a number, but it either can't fit in an unsigned
			 * long, or is too big for an unsigned short
			 */
			fprintf(stderr, "%s - value out of range\n", portStr);
			usage();
	}
	return 1;
}

void parseArgs(serverInputs * inputsStruct, int argc, char * argv[]){
	if (argc != 4) usage();

	/*Parse Port*/
	if (is_valid_port(argv[1])){
		inputsStruct->port = (u_short) strtoul(argv[1], NULL, 10);
	}
  
	/*Parse documents DIR*/
	if (is_valid_path(argv[2])){
		inputsStruct->directory = argv[2];
	}

	/* Copy the log DIR let the logger
	 * helper figure out if it is valid
	 */
	 inputsStruct->logPath = argv[3];
}

static void usage()
{
	extern char * __progname;
	fprintf(stderr, "usage: %s portnumber documentsDir logfile\n", __progname);
	exit(1);
}

int init_socket(serverInputs *inputs){
	struct sockaddr_in sockname;

	/* initialize sockname */
	memset(&sockname, 0, sizeof(sockname));
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(inputs->port);
	sockname.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int sd=socket(AF_INET,SOCK_STREAM,0);
	if ( sd == -1)
		err(1, "socket failed");

	if (bind(sd, (struct sockaddr *) &sockname, sizeof(sockname)) == -1)
		err(1, "bind failed");

	if (listen(sd,3) == -1)
		err(1, "listen failed");

	return sd;
}

int accept_connection(int socket){
	struct sockaddr_in client;
	int clientsd;

	socklen_t clientlen = sizeof(&client);
	clientsd = accept(socket, (struct sockaddr *)&client, &clientlen);
	if (clientsd == -1)
		err(1, "accept failed");

	return clientsd;
}