#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
    int sd;
	logger * logg;
	

	parseArgs(&inputs, argc, argv);
	printf("Log File Path: %s\n", inputs.logPath);
	printf("Documents Path: %s\n", inputs.directory);
	logg = init_logger(inputs.logPath);

	sd = init_socket(&inputs);

	printf("Server up and  listening on port: %d\n", inputs.port);

	daemon(1,1);

	for(;;) {
		request* req = malloc(sizeof(request));
		pthread_t thread;
		memset(req, 0, sizeof(*req));
		req->l = logg;
		int *clientsd = malloc(sizeof(int));
		*clientsd = accept_connection(sd, &(req->client));
		req->inputsDIR = inputs.directory;
		
		req->requestSD = clientsd;
		pthread_create(&thread, NULL, thread_starter, (void*)req);
	}
	
}
