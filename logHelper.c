#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "logHelper.h"

sem_t mutex;

logger* init_logger(char * file){
	validate_log_path(file);
	logger* logStr = (logger*)malloc(sizeof(logger));
	logStr->filepath = file;
	return logStr;
}

void validate_log_path(char * file){
	if (!is_valid_log_path(file)){
		printf("Log Path is Invalid or Access is denied");
		exit(1);
	}
}

int is_valid_log_path(char *file){
		FILE *f = fopen(file, "r+");
		if (f = NULL){
			return 0;
		}
		close(f);
		return 1;
}

void write_log(logger * logWirter){
	//Implement the log writing
}
