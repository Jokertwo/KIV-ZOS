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
		pwd();
		printf(":>");

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
			if ((tempPosition = parsePath(commands[1], true)) == NULL) {
				printf("PATH NOT FOUND (neexistujici cesta)\n");
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
				if (size > 0) {
					//alokuju si pamet pro cestu(o jedna vetsi pro ukoncujici znak)
					path = calloc(size + 1, sizeof(char));
					//vyjmu si cestu z argumentu
					strncpy(path, commands[1], size);
				} else {
					path = calloc(2, sizeof(char));
					strcpy(path, "/");
				}
				//cestu si projdu a zjistim tak jestli existuje
				if ((tempPosition = parsePath(path, true)) == NULL) {
					printf("PATH NOT FOUND (neexistujici cesta)\n");
					free(path);
					return FALSE;
				}
				//cestu jiz nepotrebuji
				free(path);
				//odstranim si z nazvu lomitko
				fileName++;
				//pokusim se vytvorit slozku a pritom zkontroluji jestli uz neexistuje
				if (mkdir(tempPosition, fileName) == TRUE) {
					printf("OK\n");
					return TRUE;
				}

			}
			//nebo vytvorim slozku tam kde jsem
			else {
				tempPosition = position;
				//pokusim se vytvorit slozku a pritom zkontroluji jestli uz neexistuje
				if (mkdir(tempPosition, commands[1]) == TRUE) {
					printf("OK\n");
					return TRUE;
				}
			}
			return FALSE;
		}
		printf("MISSING ARGUMENT (chybejici argument\n");
		return FALSE;
	}
	if (strcmp(commands[0], "rm") == 0) {
		Mft_Item *file;
		if (commands[1] == NULL) {
			printf("MISSING ARGUMENT (chybejici argument)\n");
			return FALSE;
		}
		if ((file = parsePath(commands[1], false)) == NULL) {
			printf("FILE NOT FOUND\n");
			return FALSE;
		}
		if (rmdir(file) == FALSE) {
			return FALSE;
		}
		printf("OK\n");
		return TRUE;

	}
	if (strcmp(commands[0], "cat") == 0) {
		Mft_Item *file;
		if (commands[1] == NULL) {
			printf("MISSING ARGUMENT (chybejici argument)\n");
			return FALSE;
		}
		if ((file = parsePath(commands[1], false)) == NULL) {
			printf("FILE NOT FOUND (neni zdroj)\n");
		}
		cat(file);
		return TRUE;
	}
	if (strcmp(commands[0], "cd") == 0) {
		if (commands[1] == NULL) {
			printf("MISSING ARGUMENT (chybejici argument)\n");
			return FALSE;
		}
		if (cd(commands[1]) == FALSE) {
			printf("PATH NOT FOUND (neexistujici cesta)\n");
			return FALSE;
		}
		printf("OK\n");
		return TRUE;
	}
	if (strcmp(commands[0], "rmdir") == 0) {
		Mft_Item *dir = NULL;
		if (commands[1] == NULL) {
			printf("MISSING ARGUMENT (chybejici argument)\n");
			return FALSE;
		}
		//pokud cesta je spravne
		if ((dir = parsePath(commands[1], true)) == NULL) {
			printf("PATH NOT FOUND (neexistujici cesta)");
			return FALSE;
		}
		//pokud je slozka prazdna
		if (isDirEmpty(dir) == FALSE) {
			printf("NOT EMPTY (adresar obsahuje podaresare, nebo soubory)\n");
			return FALSE;
		}
		//zkusim ji odstranit
		if (rmdir(dir) == FALSE) {
			return FALSE;
		}

		return TRUE;
	}
	if (strcmp(commands[0], "incp") == 0) {
		FILE *file;
		char *newFile;
		char *path;
		Mft_Item *dir;
		//overim dostatek argumentu
		if (commands[1] == NULL || commands[2] == NULL) {
			printf("MISSING ARGUMENT (chybejici argument)\n");
			return FALSE;
		}
		//overim jestli soubor existuje
		if ((file = fopen(commands[1], "r")) == NULL) {
			printf("FILE NAME NOT FOUND (neni zdroj)\n");
			return FALSE;
		}
		//hledam jestli byla zadana cesta do slozky
		if ((newFile = strrchr(commands[2], '/')) != NULL) {
			//odriznu si cestu
			int size = newFile - commands[2];
			//alokuju si pamet pro cestu(o jedna vetsi pro ukoncujici znak)
			path = calloc(size + 1, sizeof(char));
			//vyjmu si cestu z argumentu
			strncpy(path, commands[2], size);
			//cestu si projdu a zjistim tak jestli existuje
			if ((dir = parsePath(path, true)) == NULL) {
				printf("PATH NOT FOUND (neexistuje cilova cesta)\n");
				free(path);
				return FALSE;
			}
			//cestu jiz nepotrebuji
			free(path);
			//odstranim si z nazvu lomitko
			newFile++;
			if (incp(newFile, dir, file) == FALSE) {
				printf("Problem\n");
				return FALSE;
			}

		} else {
			//nebo vztvorim soubor tam kde jsem
			dir = position;
			if (incp(commands[2], dir, file) == FALSE) {
				printf("Problem\n");
				return FALSE;
			}
		}
		printf("OK\n");
		return TRUE;
	}
	if (strcmp(commands[0], "info") == 0) {
		Mft_Item *temp;
		if (commands[1] == NULL) {
			printInfoAboutMftItem(position);
			return TRUE;
		}
		if ((temp = parsePath(commands[1], true)) == NULL) {
			if ((temp = parsePath(commands[1], false)) == NULL) {
				printf("FILE NAME NOT FOUND (neni zdroj)\n");
				return FALSE;
			}
		}
		printInfoAboutMftItem(temp);
		return TRUE;
	}

	if (strcmp(commands[0], "-") == 0) {
		debugs("Konec");
		return END;
	}
	if (strcmp(commands[0], "pwd") == 0) {
		pwd();
		printf("\n");
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
