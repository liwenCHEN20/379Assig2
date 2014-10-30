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
	send_bad_request(1);

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


	send_internal_service_error(req->requestSD);
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

int send_text(int sd, char * text){
	ssize_t written, w;

	w = 0;
	written = 0;
	while (written < strlen(text)) {
		w = write(sd, text + written,
		    strlen(text) - written);
		if (w == -1) {
			if (errno != EINTR)
				err(1, "write failed");
		}
		else
			written += w;
	}
}

/* THESE SHOULD ALL BE IN TEXT FILES NOT IN SOURCE */

int send_bad_request(int sd){
	char * output = "HTTP/1.1 400 Bad Request\n\
Date: Mon 21 Jan 2008 18:06:16 GMT\n\
Content‐Type: text/html\n\
Content‐Length: 107\n\
\n\
<html><body>\n\
<h2>Malformed Request</h2>\n\
Your browser sent a request I could not understand.\n\
</body></html>";
	send_text(sd, output);
}

int send_file_not_found(int sd){
	char * output = "HTTP/1.1 404 Not Found\n\
Date: Mon 21 Jan 2008 18:06:16 GMT\n\
Content‐Type: text/html\n\
Content‐Length: 117\n\
\n\
<html><body>\n\
<h2>Document not found</h2>\n\
You asked for a document that doesn't exist. That is so sad.\n\
</body></html>";
	send_text(sd, output);
}

int send_permission_denied(int sd){
	char * output = "HTTP/1.1 403 Forbidden\n\
Date: Mon 21 Jan 2008 18:06:16 GMT\n\
Content‐Type: text/html\n\
Content‐Length: 130\n\
\n\
<html><body>\n\
<h2>Permission Denied</h2>\n\
You asked for a document you are not permitted to see. It sucks to be you.\n\
</body></html>";
	send_text(sd, output);
}

int send_internal_service_error(int sd){
char * output = "HTTP/1.1 500 Internal Server Error\n\
Date: Mon 21 Jan 2008 18:06:16 GMT\n\
Content‐Type: text/html\n\
Content‐Length: 131\n\
\n\
<html><body>\n\
<h2>Oops. That Didn't work</h2>\n\
I had some sort of problem dealing with your request. Sorry, I'm lame.\n\
</body></html>";
	send_text(sd, output);
}