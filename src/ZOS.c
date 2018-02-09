/*
 ============================================================================
 Name        : ZOS.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "createBootFile.h"
#include "commandLine.h"
void create(pthread_t a);
int load(pthread_t a);
void end(int sig);

int main(int args, char **argv) {
	pthread_t b;
	if (args == 2) {
		strcpy(fileName, argv[1]);
	} else {
		strcpy(fileName, "MyNTFS.bin");
		create(b);
	}

	debug = 0;

	if(load(b) == FALSE){
		printf("Nelze nacist soubor");
		return 1;
	}
	(void) signal(SIGINT, end);

	commandLine(NULL);
	clean();

	return EXIT_SUCCESS;
}
void end(int sig) {
	printf("Ukoncuji pseudofile system");
	clean();
	(void) signal(SIGINT, SIG_DFL);
	exit(1);

}
int load(pthread_t a) {
	void *b;
	pthread_create(&a, NULL, readBoot, NULL);
	pthread_join(a, &b);
	return (int)b;
}

void create(pthread_t a) {
	pthread_create(&a, NULL, createTextFile, NULL);
	pthread_join(a, NULL);
}
