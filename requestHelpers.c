#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <fcntl.h>
#include "requestHelpers.h"

#define BUFFSIZE 1024

void handle_request(request * req, char * docPath){
	char inbuffer[1024];
	char * tokens[4];

	read(req->requestSD, inbuffer, sizeof(inbuffer));
	parse_request(inbuffer, tokens);
	req->parsedReq = tokens;
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
		send_bad_request(req);
	}else {
		send_file(req, docPath, tokens[1]);
	}

}


int is_valid_request(char ** tokens){

	
	if (strcmp(tokens[0], "GET") != 0){
		return 0;
	}

	if (strcmp(tokens[2], "HTTP/1.1") != 0){
		return 0;
	}
	return 1;
}

int parse_request(char * req, char **tokens){
	char * line;
	char * tok;

	/*Get the first line*/
	line = strtok(req, "\r");

	tokens[3] = (char *)malloc(strlen(line) + 1);
	strcpy(tokens[3], line);

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

FILE * open_file(char * docDIR, char * filePath, int * error){
	errno = 0;
	struct stat buf;
	FILE * file;
	if(filePath[0] == '/') filePath++;


	/*create the full path*/
	char * fullPath = (char *)malloc(strlen(docDIR) + strlen(filePath) + 1);

	strcpy(fullPath, docDIR);
	strcat(fullPath, filePath);

	/*check to make sure we arent opening a dir*/
	if(stat(fullPath, &buf) != 0 || S_ISDIR(buf.st_mode)){
		*error = errno;
		return NULL;
	}

	/* now we attempt to open */
	file = fopen(fullPath, "r");
	*error = errno;
	free(fullPath);

	/* Reduce the chance of thread race
	 * for errno by copying it. Still not perfect.
	 */
	return file;
}

int read_file(FILE * fd, char ** buffer){
	long fileSize;

	fseek(fd, 0, SEEK_END);
	fileSize = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	*buffer = (char *)malloc(fileSize + 1);

	fread(*buffer, 1, fileSize, fd);
	return fileSize;
}

int send_file(request * req, char * docDIR, char * filePath){
	int error;
	char *buffer;
	FILE * file = NULL; 
	int responseSize;
	char * response;

	file = open_file(docDIR, filePath, &error);
	
	/* catch errors */
	if (file == NULL){
		switch (error){
			case ENOENT:
				send_file_not_found(req);
				break;
			case EACCES:
				send_permission_denied(req);
				break;
			default:
				send_internal_service_error(req);
				break;
		}
		return;
	}

	/* read in the file */
	responseSize = read_file(file, &buffer);

	response = get_good_response(responseSize, buffer);

	send_text(req->requestSD, response);

}

/* THESE SHOULD ALL BE IN TEXT FILES NOT IN SOURCE 
 * may not have time to remove before submission.
 */

int send_bad_request(request * req){
	int retValue;
	char * output = "HTTP/1.1 400 Bad Request\n\
Date: Mon 21 Jan 2008 18:06:16 GMT\n\
Content‐Type: text/html\n\
Content‐Length: 107\n\
\n\
<html><body>\n\
<h2>Malformed Request</h2>\n\
Your browser sent a request I could not understand.\n\
</body></html>";
	retValue = send_text(req->requestSD, output);
	write_log(req->l, (req->parsedReq)[3], inet_ntoa((req->client).sin_addr), "400 Bad Request");
	return retValue;
}

int send_file_not_found(request * req){
	int retValue;
	char * output = "HTTP/1.1 404 Not Found\n\
Date: Mon 21 Jan 2008 18:06:16 GMT\n\
Content‐Type: text/html\n\
Content‐Length: 117\n\
\n\
<html><body>\n\
<h2>Document not found</h2>\n\
You asked for a document that doesn't exist. That is so sad.\n\
</body></html>";
	retValue = send_text(req->requestSD, output);
	write_log(req->l, (req->parsedReq)[3], inet_ntoa((req->client).sin_addr), "404 Not Found");
	return retValue;
}

int send_permission_denied(request * req){
	int retValue;
	char * output = "HTTP/1.1 403 Forbidden\n\
Date: Mon 21 Jan 2008 18:06:16 GMT\n\
Content‐Type: text/html\n\
Content‐Length: 130\n\
\n\
<html><body>\n\
<h2>Permission Denied</h2>\n\
You asked for a document you are not permitted to see. It sucks to be you.\n\
</body></html>";
	retValue = send_text(req->requestSD, output);
	write_log(req->l, (req->parsedReq)[3], inet_ntoa((req->client).sin_addr), "403 Forbidden");
	return retValue;
}

int send_internal_service_error(request * req){
	int retValue;
	char * output = "HTTP/1.1 500 Internal Server Error\n\
Date: Mon 21 Jan 2008 18:06:16 GMT\n\
Content‐Type: text/html\n\
Content‐Length: 131\n\
\n\
<html><body>\n\
<h2>Oops. That Didn't work</h2>\n\
I had some sort of problem dealing with your request. Sorry, I'm lame.\n\
</body></html>";
	retValue = send_text(req->requestSD, output);
	write_log(req->l, (req->parsedReq)[3], inet_ntoa((req->client).sin_addr), "500 Internal Server Error");
	return retValue;
}

char * get_good_response(int contentLen, char * content){
	char * header;
	char * lengthLine;
	char * contentLine;
	char * fullOutput;
	int totalLength;
	int partialLength;

	header = "HTTP/1.1 200 OK\n\
Date: Mon 21 Jan 2008 18:06:16 GMT\n\
Content-Type: text/html\n";

	lengthLine = "Content-Length:";


	partialLength = snprintf(NULL, 0,"%s %d\n\n", lengthLine, contentLen);


	contentLine = (char *)malloc(partialLength + 1);


	sprintf(contentLine, "%s %d\n\n", lengthLine, contentLen);


	fullOutput = (char *) malloc(strlen(contentLine) + 
		strlen(header) + 
		contentLen + 2);


	sprintf(fullOutput, "%s%s%s", header, contentLine, content);


	return fullOutput;
}