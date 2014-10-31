#ifndef LOGHELPER_H
#define LOGHELPER_H

typedef struct{
	char * filepath;
} logger;

logger* init_logger(char *);

int is_valid_log_path(char *);

void write_log(logger *, char *, char *, char *);

void validate_log_path(char * );

#endif