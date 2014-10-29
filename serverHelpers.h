#ifndef SERVERHELPERS_H
#define SERVERHELPERS_H

#include <sys/types.h>

typedef struct{
	u_short port;
	char *directory;
	char *logPath;
}serverInputs;

int is_valid_path(char *);

int is_valid_port(char *);

void parseArgs(serverInputs *, int , char **);

static void usage();

int init_socket(serverInputs *);

int accept_connection(int);

#endif