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

int  parse_request(char *, char **);

int clean_up_tokens_array(char **);

#endif