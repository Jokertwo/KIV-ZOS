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

		if (strlen(inputLine) == 0) {
			continue;
		}
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
		//pokud nebyl dan argument vypisu obsah slozky kde jsem
		if (commands[1] == NULL) {
			ls(position);
		} else {
			Mft_Item *tempPosition;
			//zkusim najit cestu do zadane slozky
			if ((tempPosition = parsePath(commands[1])) == NULL) {
				printf("PATH NOT FOUND (neexistujici cesta)");
				return FALSE;
			}
			//vypisu obsah slozky
			ls(tempPosition);
		}
		return TRUE;
	}
	if (strcmp(commands[0], "mkdir") == 0) {
		char *fileName = NULL;
		char *path;
		int size;
		Mft_Item *tempPosition;
		//byl zadan nejaky argument
		if (commands[1] != NULL) {
			//hledam jestli byla zadana cesta do slozky
			if ((fileName = strrchr(commands[1], '/')) != NULL) {
				//odriznu si cestu
				size = fileName - commands[1];
				//alokuju si pamet pro cestu(o jedna vetsi pro ukoncujici znak)
				path = calloc(size + 1, sizeof(char));
				//vyjmu si cestu z argumentu
				strncpy(path, commands[1], size);
				//cestu si projdu a zjistim tak jestli existuje
				if ((tempPosition = parsePath(path)) == NULL) {
					printf("PATH NOT FOUND (neexistujici cesta)");
					free(path);
					return FALSE;
				}
				//cestu jiz nepotrebuji
				free(path);
				//odstranim si z nazvu lomitko
				fileName++;
				//pokusim se vytvorit slozku a pritom zkontroluji jestli uz neexistuje
				mkdir(tempPosition, fileName);

			}
			//nebo vytvorim slozku tam kde jsem
			else {
				tempPosition = position;
				fileName = calloc(strlen(commands[1]) + 1, sizeof(char));
				strcpy(fileName, commands[1]);
				//pokusim se vytvorit slozku a pritom zkontroluji jestli uz neexistuje
				mkdir(tempPosition, fileName);
				free(fileName);
			}
			return TRUE;
		}
		printf("MISSING ARGUMENT (chybejici argument\n");
	}
	if (strcmp(commands[0], "cd") == 0) {
		if (commands[1] == NULL) {
			printf("MISSING ARGUMENT (chybejici argument)\n");
			return FALSE;
		}
		if (cd(commands[1]) == FALSE) {
			printf("PATH NOT FOUND (neexistujici cesta)");
			return FALSE;
		}
		printf("OK\n");
		return TRUE;
	}
	if (strcmp(commands[0], "rmdir") == 0) {
		if (commands[1] == NULL) {
			printf("MISSING ARGUMENT (chybejici argument)\n");
			return FALSE;
		}

		Mft_Item *dir = NULL;
		//pokud cesta je spravne
		if ((dir = parsePath(commands[1])) == NULL) {
			printf("PATH NOT FOUND (neexistujici cesta)");
			return FALSE;
		}

		//pokud je cilem slozka
		if (dir->isDirectory == true) {
			//zkusim ji odstranit
			if (rmdir(dir) == FALSE) {
				printf(
						"NOT EMPTY (adresar obsahuje podaresare, nebo soubory)\n");
				return FALSE;
			}
		}
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
