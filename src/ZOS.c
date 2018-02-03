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

#include "createBootFile.h"
#include "commandLine.h"
void create(pthread_t a);
void load(pthread_t a);
int main(void) {

	debug = 1;
	pthread_t b;

	//create(b);
	load(b);

	pthread_create(&b, NULL, commandLine, NULL);
	pthread_join(b, NULL);



	clean();

	return EXIT_SUCCESS;
}
void load(pthread_t a){
	pthread_create(&a, NULL, readBoot, NULL);
		pthread_join(a, NULL);
}

void create(pthread_t a) {
	pthread_create(&a, NULL, createTextFile, NULL);
	pthread_join(a, NULL);
}
