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
#include<signal.h>

#include "createBootFile.h"
#include "commandLine.h"
void create(pthread_t a);
void load(pthread_t a);
void end(int sig);

int main(void) {

	debug = 0;
	pthread_t b;

	create(b);
	load(b);
	(void) signal(SIGINT, end);
	pthread_create(&b, NULL, commandLine, NULL);
	pthread_join(b, NULL);

	clean();

	return EXIT_SUCCESS;
}
void end(int sig) {
	printf("Ukoncuji pseudofile system");
	clean();
	(void) signal(SIGINT, SIG_DFL);
	exit(1);

}
void load(pthread_t a) {
	pthread_create(&a, NULL, readBoot, NULL);
	pthread_join(a, NULL);
}

void create(pthread_t a) {
	pthread_create(&a, NULL, createTextFile, NULL);
	pthread_join(a, NULL);
}
