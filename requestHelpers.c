#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fcntl.h>
#include "requestHelpers.h"

/* This controls how much of a file is read into mem
 * and sent at one time to a client. Increase for greater
 * speed AND memory usage */
#define BUFFSIZE 30

void * thread_starter (void * req){
	handle_request((request *) req);
	free(((request *)req)->requestSD);
	free(req);
}

void handle_request(request * req){
	char inbuffer[1024];
	char * tokens[4];

	read(*(req->requestSD), inbuffer, sizeof(inbuffer));

	//We got a null request
	if (parse_request(inbuffer, tokens) == 0){
		close(*(req->requestSD));
		return;
	}

	
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
	req->parsedReq = tokens;

	if (!is_valid_request(tokens)){
		send_bad_request(req);
	}else {
		send_file(req, req->inputsDIR, tokens[1]);
	}

	
	free_array(tokens, 4);
	close(*(req->requestSD));
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
	char * saveptr;

	/*Get the first line*/
	printf("len of line before segf: %d\n", strlen(req));
	printf("Request before segf: %s\n", req);
	line = strtok_r(req, "\r", &saveptr);
	printf("Line before SEGF: %s\n", line);

	if (line == NULL){
		//Need a better way to do this
		tokens[3] = "<unusable or empty request>";
		return 0;
	}

	tokens[3] = (char *)malloc(strlen(line) + 1);
	strcpy(tokens[3], line);

	/*Get firs token */
	tok = strtok_r(line, " ", &saveptr);
	tokens[0] = (char *)malloc(strlen(tok) + 1);
	strcpy(tokens[0], tok);

	/*Get second token*/
	tok = strtok_r(NULL, " ", &saveptr);
	tokens[1] = (char *)malloc(strlen(tok) + 1);
	strcpy(tokens[1], tok);
	
	/* Get last token */
	tok = strtok_r(NULL, " ", &saveptr);
	tokens[2] = (char *)malloc(strlen(tok) + 1);
	strcpy(tokens[2], tok);

	return 3;
}

void free_array(char ** tokens, int len){
	int i;
	for ( i=0; i < len; i++){
		free(tokens[i]);
	}
}

int send_text(int sd, char * text){
	ssize_t written, w;

	w = 0;
	written = 0;
	while (written < strlen(text)) {
		w = write(sd, text + written,
		    strlen(text) - written);
		if (w == -1) {
			if (errno != EINTR){
				//err(1, "write failed");
				return 0;
			}
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


int send_file(request * req, char * docDIR, char * filePath){
	int error;
	char *buffer;
	FILE * file = NULL; 
	unsigned int responseSize;
	unsigned int sentSize;
	char * response;

	file = open_file(docDIR, filePath, &error);
	
	/* catch errors */
	if (file == NULL){
		switch (error){
			case ENOENT:
				send_file_not_found(req);
				return 0;
			case EACCES:
				send_permission_denied(req);
				return 0;
			default:
				send_internal_service_error(req);
				return 0;
		}
		return 1;
	}

	responseSize = get_file_size(file);
	response = get_good_response(responseSize);
	printf("ressponse: %s\n", response);
	send_text(*(req->requestSD), response);

	sentSize = transfer_file(file, responseSize, *(req->requestSD));
	write_good_response(req, responseSize, sentSize);

	/*close and free resources */
	fclose(file);
	free(response);

}

int transfer_file(FILE *file, int length, int sd){
	char buffer[BUFFSIZE];
	int read = 0;
	int written = 0;

	while(read < length){
		memset(buffer, 0, BUFFSIZE);
		read += fread(buffer, 1, BUFFSIZE, file);
		if (!send_text(sd, buffer)) break;
	}
	return read;
}

long get_file_size(FILE *fd){
	long fileSize;

	fseek(fd, 0, SEEK_END);
	fileSize = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	return fileSize;
}

/* Big strings should be in txt files not source
 * may not have time to remove before submission.
 */

int write_good_response(request * req, long size, long sent){
	char output[256];
	sprintf(output, "200 OK %d/%d", sent, size);
	write_log(req->l, (req->parsedReq)[3], inet_ntoa((req->client).sin_addr), output);
}

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
	retValue = send_text(*(req->requestSD), output);
	printf("before seg fault log: %s\n", req->parsedReq[3]);
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
	retValue = send_text(*(req->requestSD), output);
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
	retValue = send_text(*(req->requestSD), output);
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
	retValue = send_text(*(req->requestSD), output);
	write_log(req->l, (req->parsedReq)[3], inet_ntoa((req->client).sin_addr), "500 Internal Server Error");
	return retValue;
}

char * get_good_response(int contentLen){
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
		strlen(header) + 2);


	sprintf(fullOutput, "%s%s", header, contentLine);

	free(contentLine);

	return fullOutput;
}