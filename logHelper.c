#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <semaphore.h>
#include "logHelper.h"

#define MUTEXNAME logger_mutex

static sem_t* mutex;

logger* init_logger(char * file){
	validate_log_path(file);
	mutex = sem_open("MUTEXNAME5", O_CREAT, 0600, 1);
	logger* logStr = (logger*)malloc(sizeof(logger));
	logStr->filepath = file;
	printf("logger attached to: %s\n", file);
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

void write_log(logger * logWirter, char * req, char * clientAddr, char * resp){
	//Implement the log writing
	printf("waiting to enter mutex\n");
	sem_t* sem = sem_open("MUTEXNAME5", O_RDWR);
	sem_wait(sem);
	printf("in mutex\n");
	time_t t;
	time(&t);

	printf("attempt to open log file:\n");

	FILE * file = fopen(logWirter->filepath, "r+");
	if (file != NULL){
		fprintf(file, "%s\t%s\t%s\t%s\n", ctime(&t), clientAddr, req, resp);
		close(file);
	}
	printf("leaving mutex\n");
	sem_post(sem);
}


void dealloc_logger(logger * l){
	//Free some resources
	sem_close(mutex);
	free(l);
}