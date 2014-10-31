#ifndef REQUESTHELPERS_H
#define REQUESTHELPERS_H

#include "logHelper.h"

typedef struct {
	int requestSD;
	char * requesstBuffer;
	logger * l;
}request;

void handle_request(request *, char *);

int is_valid_request(char **);

int parse_request(char *, char **);

FILE * open_file(char *, char *, int *);

int read_file(FILE *, char **);

int send_file(int, char *, char *);

int send_bad_request(int);

int send_file_not_found(int);

int send_permission_denied(int);

int send_internal_service_error(int);

int send_text(int, char *);

char * get_good_response(int, char *);

#endif