#ifndef REQUESTHELPERS_H
#define REQUESTHELPERS_H

#include "logHelper.h"

typedef struct {
	int requestSD;
	char * requesstBuffer;
	logger * l;
}request;

void handle_request(request *);

int is_request_valid(char * request);

#endif