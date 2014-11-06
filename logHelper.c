#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include "logHelper.h"

static sem_t* mutex;

logger* init_logger(char * file){
	validate_log_path(file);
	mutex = sem_open("logger_mutex1", O_CREAT, 0600, 1);
	logger* logStr = (logger*)malloc(sizeof(logger));
	logStr->filepath = file;
	printf("logger attached to: %s\n", file);
	return logStr;
}

void validate_log_path(char * file){
	if (!is_valid_log_path(file)){
		exit(1);
	}
}

int is_valid_log_path(char *file){
		char * pch;
		char directory[strlen(file)];

		pch = strrchr(file, '/');
		strncpy(directory, file, pch-file+1);
		DIR *d = opendir(directory);
		if (d == NULL){
			return 0;
		}
		closedir(d);
		return 1;
}

void write_log(logger * logWirter, char * req, char * clientAddr, char * resp){
	//Implement the log writing
	printf("waiting to enter mutex\n");
	sem_t* sem = sem_open("logger_mutex1", O_RDWR);
	sem_wait(sem);
	time_t t;
	time(&t);


	FILE * file = fopen(logWirter->filepath, "a+");
	if (file != NULL){
		printf("writing to log: %s\t%s\t%s\t%s\n", ctime(&t), clientAddr, req, resp);
		fprintf(file, "%s\t%s\t%s\t%s\n", ctime(&t), clientAddr, req, resp);
		fclose(file);
	} else {
		printf("couldn't write to log file\n");
	}
	printf("leaving mutex\n");
	sem_post(sem);
}


void dealloc_logger(logger * l){
	//Free some resources
	sem_close(mutex);
	free(l);
}
