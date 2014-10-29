#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "requestHelpers.h"

void handle_request(request * req){
	char buffer[80];
	char inbuffer[1024];
	ssize_t written, w;
			/*
			 * write the message to the client, being sure to
			 * handle a short write, or being interrupted by
			 * a signal before we could write anything.
			 */

	/*Lets try to print the request*/
	read(req->requestSD, inbuffer, sizeof(inbuffer));


#ifdef DEBUG

if (is_valid_request(inbuffer)){
	printf("******RECEIVED VALID REQUEST********\n");
	printf("--------------Request----------\n");
	printf("%s\n", inbuffer);
	printf("-----------End Request-----------\n");		 
} else {
	printf("!!!!!!!!RECEIVED INVALID REQUEST!!!!!!\n");
}

#endif


	/* the message we send the client */
	strncpy(buffer,
	    "What is the air speed velocity of a coconut laden swallow?\n",
	    sizeof(buffer));
	w = 0;
	written = 0;
	while (written < strlen(buffer)) {
		w = write(req->requestSD, buffer + written,
		    strlen(buffer) - written);
		if (w == -1) {
			if (errno != EINTR)
				err(1, "write failed");
		}
		else
			written += w;
	}
}

int is_valid_request(char * req){
	char * line;
	char * tok;

	//Get the first line
	line = strtok(req, "\n");
	printf("%s\n", line);

	/*First word should be GET*/
	tok = strtok(line, " ");
	printf("%s\n", tok);
	if (strcmp(tok, "GET") != 0){
		return 0;
	}

	/*Second word should be a file*/
	tok = strtok(NULL, " ");
	printf("%s\n", tok);

	/*Third should be HTTP..*/
	tok = strtok(NULL, " ");
	printf("%s\n", tok);

	if (strncmp(tok, "HTTP/1.1", 8) != 0){
		printf("Last token failed\n");
		return 0;
	}
	return 1;
}
