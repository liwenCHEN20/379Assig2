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
	printf("--------------Request----------\n");
	printf("%s\n", inbuffer);
	printf("-----------End Request-----------\n");		 

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