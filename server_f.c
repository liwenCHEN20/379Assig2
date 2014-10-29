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

#include "serverHelpers.h"
#include "requestHelpers.h"


static void kidhandler(int signum) {
	/* signal handler for SIGCHLD */
	waitpid(WAIT_ANY, NULL, WNOHANG);
}


int main(int argc,  char *argv[])
{
	serverInputs inputs;
	struct sigaction sa;
    int sd;
	pid_t pid;

	/*Parse all Arguments*/
	memset(&inputs, 0, sizeof(serverInputs));

	parseArgs(&inputs, argc, argv);

	/******* NEED TO INITIALIZE LOGGER STILL *****/

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

	/*--------SERVER F SPECIFIC----------*/
	/*
	 * first, let's make sure we can have children without leaving
	 * zombies around when they die - we can do this by catching
	 * SIGCHLD.
	 */
	sa.sa_handler = kidhandler;
        sigemptyset(&sa.sa_mask);
	/*
	 * we want to allow system calls like accept to be restarted if they
	 * get interrupted by a SIGCHLD
	 */
        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sa, NULL) == -1)
                err(1, "sigaction failed");

    /*-----------END SERVER F SPECIFIC-------------------*/

	/*
	 * finally - the main loop.  accept connections and deal with 'em
	 */
#ifdef DEBUG
printf("Server up and listening for connections on port %u\n", inputs.port);
#endif

	for(;;) {
		request req;
		int clientsd = accept_connection(sd);
		/*
		 * We fork child to deal with each connection, this way more
		 * than one client can connect to us and get served at any one
		 * time.
		 */

		memset(&req, 0, sizeof(req));
		req.requestSD = clientsd;

		pid = fork();
		if (pid == -1)
		     err(1, "fork failed");

		if(pid == 0) {
			handle_request(&req);
			exit(0);
		}
		close(clientsd);
	}
	
}
