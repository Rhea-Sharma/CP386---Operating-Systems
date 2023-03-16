/**
CP386 A4 Q1 (stack.c)
Author: Rhea Sharma & Mobina Tooranisama
ID: 200576620 & 200296720
Email: shar7662@mylaurier.ca & toor6720@mylaurier.ca
Date: March , 2023
**/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>

#define BUF 100

sem_t even;
sem_t odd;
int oddCount = 0;
int evenCount = 0;

void logStart(char* tID);
void logFinish(char* tID);

void startClock();
long getCurrentTime();
time_t programClock;

typedef struct thread {
	char tid[4];
	unsigned int startTime;
	int state;
	pthread_t handle;
	int retVal;
	int check_odd;
} Thread;

int threadsLeft(Thread* threads, int threadCount);
int threadToStart(Thread* threads, int threadCount);
void* threadRun(void* t);
int readFile(char* fileName, Thread** threads);
void checkParity(Thread* threads, int threadCount);

int main(int argc, char **argv) {

	if(argc < 2) {
		printf("Input file name missing...exiting with error code -1\n");
		return -1;
	}

	Thread* threads = NULL;
	int threadCount = readFile(argv[1],&threads);
	
	sem_init(&even, 0, 0);
	sem_init(&odd, 0, 0);

	startClock();
	checkParity(threads, threadCount);

	int i = 0;

	while(threadsLeft(threads, threadCount) > 0) {
		if (threads[i].check_odd == 1) {
			sem_post(&odd);
		} else {
			sem_post(&even);
		}
		
		while((i=threadToStart(threads, threadCount)) > -1) {
			oddCount = 0;
			evenCount = 0;

			for (int i = 0; i < threadCount; i++) {
				if (threads[i].state != -1 && threads[i].check_odd == 1) {
					oddCount++;
				}
			}

			for (int j = 0; j < threadCount; j++) {
				if (threads[j].state != -1 && threads[j].check_odd == 0) {
					evenCount++;
				}
			}

			threads[i].state = 1;
			threads[i].retVal = pthread_create(&(threads[i].handle), NULL, threadRun, &threads[i]);
		}

		if (oddCount == 0) { 
			sem_post(&even);
		} else if (evenCount == 0) {
			sem_post(&odd);
		}
	}

	sem_destroy(&even);
	sem_destroy(&odd);

	return 0;
}

int readFile(char* fileName, Thread** threads) {
	FILE *in = fopen(fileName, "r");
	if(!in) {
		printf("Child A: Error in opening input file...exiting with error code -1\n");
		return -1;
	}

	struct stat st;
	fstat(fileno(in), &st);
	char* fileContent = (char*) malloc(((int)st.st_size + 1) * sizeof(char));
	fileContent[0]='\0';	

	while(!feof(in)) {
		char line[BUF];
		if(fgets(line, BUF, in) != NULL) {
			strncat(fileContent, line, strlen(line));
		}
	}

	fclose(in);

	char* command = NULL;
	int threadCount = 0;

	char* fileCopy = (char*) malloc((strlen(fileContent) + 1) * sizeof(char));
	strcpy(fileCopy, fileContent);
	
	command = strtok(fileCopy, "\r\n");
	
	while(command != NULL) {
		threadCount++;
		command = strtok(NULL, "\r\n");
	}

	*threads = (Thread*) malloc(sizeof(Thread) * threadCount);

	int i = 0;
	char* lines[threadCount];
	command = NULL;
	command = strtok(fileContent, "\r\n");

	while(command != NULL) {
		lines[i] = malloc (sizeof(command) * sizeof(char));
		strcpy(lines[i], command);
		i++;
		command = strtok(NULL, "\r\n");
	}

	for(int j = 0; j < threadCount; j++) {
		char* token = NULL;
		token = strtok(lines[j], ";");
		int count = 0;

		while(token != NULL) {
			(*threads)[j].state = 0;

			if(count == 0) {
				strcpy((*threads)[j].tid, token);
			}
			
			else if(count == 1) {
				(*threads)[j].startTime = atoi(token);
			}

			count++;
			token = strtok(NULL, ";");
		}
	}

	return threadCount;
}

void logStart(char* tID) {
	printf("[%ld] New Thread with ID %s is started.\n", getCurrentTime(), tID);
}

void logFinish(char* tID) {
	printf("[%ld] Thread with ID %s is finished.\n", getCurrentTime(), tID);
}

void checkParity(Thread* threads, int threadCount) {
	for(int i = 0; i < threadCount; i++)
	{
		int threadValue = (int)threads[i].tid[strlen(threads[i].tid) - 1];
		if(threadValue % 2 != 0) {
			threads[i].check_odd = 1;
		} else {
			threads[i].check_odd = 0;
		}
	}
}

int threadsLeft(Thread* threads, int threadCount) {
	int counter = 0;

	for(int i = 0; i < threadCount; i++) {
		if(threads[i].state > -1) {
			counter++;
		}
	}

	return counter;
}

int threadToStart(Thread* threads, int threadCount) {
	for(int i = 0; i < threadCount; i++) {
		if(threads[i].state == 0 && threads[i].startTime == getCurrentTime()) {
			return i;
		}
	}

	return -1;
}

void* threadRun(void* t) {
	logStart(((Thread*)t)->tid);

	if (((Thread*)t)->check_odd == 1) {
		sem_wait(&odd);
	} else {
		sem_wait(&even);
	}

	printf("[%ld] Thread %s is in its critical section\n",getCurrentTime(), ((Thread*)t)->tid);
	
	if (((Thread*)t)->check_odd == 0) {
		sem_post(&odd);
	} else {
		sem_post(&even);
	}

	logFinish(((Thread*)t)->tid);

	((Thread*)t)->state = -1;
	pthread_exit(0);
}

void startClock() {
	programClock = time(NULL);
}

long getCurrentTime() {
	time_t now;
	now = time(NULL);
	return now-programClock;
}