#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "requestHelpers.h"

void handle_request(request * req){
	char buffer[80];
	char inbuffer[1024];
	char * tokens[3];
	ssize_t written, w;
			/*
			 * write the message to the client, being sure to
			 * handle a short write, or being interrupted by
			 * a signal before we could write anything.
			 */


	read(req->requestSD, inbuffer, sizeof(inbuffer));
	parse_request(inbuffer, tokens);


#ifdef DEBUG
	
	/*Lets try to print the request*/
if (is_valid_request(tokens)){
	printf("******RECEIVED VALID REQUEST********\n");
	printf("--------------Request----------\n");
	printf("%s\n", tokens[1]);
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


int is_valid_request(char ** tokens){

	
	if (strcmp(tokens[0], "GET") != 0){
		return 0;
	}

	if (strcmp(tokens[2], "HTTP/1.1") != 0){
		printf("Last token failed\n");
		return 0;
	}
	return 1;
}

int parse_request(char * req, char **tokens){
	char * line;
	char * tok;

	/*Get the first line*/
	line = strtok(req, "\r");

	/*Get firs token */
	tok = strtok(line, " ");
	tokens[0] = (char *)malloc(strlen(tok) + 1);
	strcpy(tokens[0], tok);

	/*Get second token*/
	tok = strtok(NULL, " ");
	tokens[1] = (char *)malloc(strlen(tok) + 1);
	strcpy(tokens[1], tok);
	
	/* Get last token */
	tok = strtok(NULL, " ");
	tokens[2] = (char *)malloc(strlen(tok) + 1);
	strcpy(tokens[2], tok);

	return 3;
}
