/*
 * commandLine.c
 *
 *  Created on: 3.2.2018
 *      Author: jokertwo
 */
#define MAX 200
#define END -1000
#include "commandLine.h"

int functions(char *command);
void printHelp(void);
void *commandLine(void *args) {

	//buffer pro prikaz
	char command[MAX];

	while (true) {
		memset(command, 0, MAX);
		printf("Zadej prikaz:\n");

		fgets(command, MAX, stdin);
		command[strlen(command) - 1] = '\0';

		if (functions(strtok(command, " ")) == END) {
			break;
		}
	}
	return (int*) TRUE;
}

int functions(char *command) {

	if (strcmp(command, "ls") == 0) {
		ls(position->uid);
		return TRUE;
	}
	if (strcmp(command, "mkdir") == 0) {
		mkdir(position->uid,strtok(NULL," "));
		return TRUE;
	}
	if(strcmp(command, "cd") == 0){
		if(cd(strtok(NULL, " ")) == FALSE){
			printf("PATH NOT FOUND (neexistujici cesta)");
			return FALSE;
		}
		return TRUE;
	}
	if (strcmp(command, "-") == 0) {
		debugs("Konec");
		return END;
	}
	if(strcmp(command, "pwd") == 0){
		pwd();
		return TRUE;
	}
	if (strcmp(command, "help") == 0) {
		printHelp();
		return TRUE;
	}

	printf("Unknow command %s \nType in 'help'\n", command);
	return TRUE;
}

void printHelp(void) {
	printf("Tady bude napoveda");
}
