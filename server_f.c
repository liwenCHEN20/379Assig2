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
	logger * logg;
	/*Parse all Arguments*/
	memset(&inputs, 0, sizeof(serverInputs));

	parseArgs(&inputs, argc, argv);
	logg = init_logger(inputs.logPath);

	sd = init_socket(&inputs);

	sa.sa_handler = kidhandler;
        sigemptyset(&sa.sa_mask);

        sa.sa_flags = SA_RESTART;
        if (sigaction(SIGCHLD, &sa, NULL) == -1)
                err(1, "sigaction failed");

    printf("Server up and  listening on port: %d\n", inputs.port);

	for(;;) {
		request req;
		memset(&req, 0, sizeof(req));
		req.l = logg;
		int clientsd = accept_connection(sd, &req.client);
		req.inputsDIR = inputs.directory;
		
		req.requestSD = &clientsd;

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
