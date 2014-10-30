#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "requestHelpers.h"

#define BUFFSIZE 1024

void handle_request(request * req){
	char inbuffer[1024];
	char * tokens[3];

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

	
	if (!is_valid_request(tokens)){
		send_bad_request(req->requestSD);
		return;
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
	return 1;
}

int open_file(char * docDIR, char * filePath, FILE * file){
	errno = 0;
	if(filePath[0] == '/') filePath++;

	/*create the full path*/
	char * fullPath = (char *)malloc(strlen(docDIR) + strlen(filePath) + 1);

	strcpy(fullPath, docDIR);
	strcat(fullPath, filePath);

	/* now we attempt to open */
	file = fopen(fullPath, "r");

	if (file == NULL){
		/* reduce the chance of race condition on 
		 * errno by copying it
		 */ 
		return errno;
	}
}

int read_file(int sd, char * buffer, int size){
	int realSize = size;
	int readSize = 0;
	char * readBuffer[size];

	fseek(sd, 0, SEEK_END);
	
}

int send_file(int sd, char * docDIR, char * filePath){
	int error;
	char *buffer[BUFFSIZE];
	FILE * file; 
	error = open_file(docDIR, filePath, file);
	
	/* catch errors */
	if (file == NULL){
		switch (error){
			case ENOENT:
				send_file_not_found(sd);
				break;
			case EACCES:
				send_permission_denied(sd);
				break;
			default:
				send_internal_service_error(sd);
				break;
		}
		return;
	}


}

/* THESE SHOULD ALL BE IN TEXT FILES NOT IN SOURCE 
 * may not have time to remove before submission.
 */

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
	return send_text(sd, output);
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
	return send_text(sd, output);
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
	return send_text(sd, output);
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
	return send_text(sd, output);
}