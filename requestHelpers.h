#ifndef REQUESTHELPERS_H
#define REQUESTHELPERS_H

#include "logHelper.h"

typedef struct {
	int requestSD;
	char * requesstBuffer;
	logger * l;
}request;

void handle_request(request *);

int is_valid_request(char **);

int parse_request(char *, char **);

int open_file(char *, char *, FILE *);

int read_file(int, char *, int);

int send_file(int, char *, char *);

int send_bad_request(int);

int send_file_not_found(int);

int send_permission_denied(int);

int send_internal_service_error(int);

int send_text(int, char *);

#endif