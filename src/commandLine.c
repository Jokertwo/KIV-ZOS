/*
 * commandLine.c
 *
 *  Created on: 3.2.2018
 *      Author: jokertwo
 */
#define MAX 200
#define END -1000
#include "commandLine.h"

int functions(int numCommand);
void printHelp(void);
void freeCommands(int numCommands);
void prepareCommands(void);

char* commands[MAX];

void prepareCommands(void) {
	for (int i = 0; i < MAX; i++) {
		commands[i] = NULL;
	}
}

void freeCommands(int numCommands) {
	for (int i = 0; i < numCommands; i++) {
		free(commands[i]);
		commands[i] = NULL;
	}
}

void *commandLine(void *args) {

	//buffer pro prikaz
	char* inputLine = (char*) malloc(MAX);
	char* input;
	int i;
	int numCommand = 0;

	prepareCommands();
	while (true) {
		memset(inputLine, 0, MAX);
		printf("Zadej prikaz:\n");

		fgets(inputLine, MAX, stdin);
		inputLine[strlen(inputLine) - 1] = '\0';

		input = strtok(inputLine, " ");
		for (i = 0; input != NULL; i++) {
			numCommand++;
			commands[i] = malloc(strlen(input) + 1);
			memset(commands[i], 0, strlen(input) + 1);
			strncpy(commands[i], input, strlen(input));
			input = strtok(NULL, " ");
		}

		if (functions(numCommand) == END) {
			freeCommands(numCommand);
			break;
		}
		freeCommands(numCommand);
		numCommand = 0;

	}
	free(inputLine);
	return (int*) TRUE;
}

int functions(int numCommand) {

	if (strcmp(commands[0], "ls") == 0) {
		if (commands[1] == NULL) {
			ls(position);
		} else {
			Mft_Item *temp;
			if ((temp = parsePath(commands[1])) == NULL) {
				printf("PATH NOT FOUND (neexistujici cesta)");
				return FALSE;
			}
			ls(temp);
		}
		return TRUE;
	}
	if (strcmp(commands[0], "mkdir") == 0) {

		mkdir(position->uid, commands[1]);
		return TRUE;
	}
	if (strcmp(commands[0], "cd") == 0) {
		if (cd(commands[1]) == FALSE) {
			printf("PATH NOT FOUND (neexistujici cesta)");
			return FALSE;
		}
		printf("OK\n");
		return TRUE;
	}

	if (strcmp(commands[0], "-") == 0) {
		debugs("Konec");
		return END;
	}
	if (strcmp(commands[0], "pwd") == 0) {
		pwd();
		return TRUE;
	}
	if (strcmp(commands[0], "help") == 0) {
		printHelp();
		return TRUE;
	}

	printf("Unknow command %s \nType in 'help'\n", commands[0]);
	return TRUE;
}

void printHelp(void) {
	printf("Tady bude napoveda");
}
