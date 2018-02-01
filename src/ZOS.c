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
#include "load.h"

int main(void) {

	pthread_t a,b;

	pthread_create(&a, NULL, createTextFile, NULL);
	pthread_join(a,NULL);
	pthread_create(&b,NULL,readBoot,NULL);
	pthread_join(b,NULL);


	return EXIT_SUCCESS;
}
