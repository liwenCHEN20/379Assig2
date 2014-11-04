#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include <err.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "serverHelpers.h"
#include "requestHelpers.h"


int main(int argc,  char *argv[])
{
	serverInputs inputs;
	struct sigaction sa;
    int sd;
	pid_t pid;
	logger * logg;
	/*Parse all Arguments*/
	memset(&inputs, 0, sizeof(serverInputs));

	parseArgs(&inputs, argc, argv);
	printf("started:\n");
	/******* NEED TO INITIALIZE LOGGER STILL *****/
	logg = init_logger(inputs.logPath);
	printf("logger init: %s\n", logg->filepath);


#ifdef DEBUG
printf("port: %d\n", (int)(inputs.port));
printf("Documents DIR: %s\n", inputs.directory);
printf("Log Path: %s\n", inputs.logPath);
#endif

	

	sd = init_socket(&inputs);

	/*
	 * we're now bound, and listening for connections on "sd" -
	 * each call to "accept" will return us a descriptor talking to
	 * a connected client
	 */

	/*
	 * finally - the main loop.  accept connections and deal with 'em
	 */
#ifdef DEBUG
printf("Server up and listening for connections on port %u\n", inputs.port);
#endif

	for(;;) {
		request* req = malloc(sizeof(request));
		pthread_t thread;
		memset(req, 0, sizeof(*req));
		req->l = logg;
		int *clientsd = malloc(sizeof(int));
		*clientsd = accept_connection(sd, &req->client);
		req->inputsDIR = inputs.directory;
		printf("connec accepted\n");

		
		req->requestSD = clientsd;
		printf("spinning off thead\n");
		pthread_create(&thread, NULL, thread_starter, (void*)req);
		//pthread_join(thread, NULL);
		printf("thread completed\n");


	}
	
}
