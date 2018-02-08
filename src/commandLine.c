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
		Resolut *res;
		//byl zadan nejaky argument
		if (commands[1] != NULL) {
			if ((res = destination(commands[1], true)) == NULL) {
				printf("PATH NOT FOUND (neexistujici cesta)\n");
				return FALSE;
			}
			if (mkdir(res->item, res->name) == TRUE) {
				printf("OK\n");
				free(res);
				return TRUE;
			}
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
			return FALSE;
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
		Resolut *res;
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
		if ((res = destination(commands[2], true)) == NULL) {
			printf("PATH NOT FOUND (neexistuje cilova cesta)\n");
			fclose(file);
			return FALSE;
		}
		if (incp(res->name, res->item, file) == FALSE) {
			fclose(file);
			free(res);
			return FALSE;
		}
		free(res);
		printf("OK\n");
		return TRUE;
	}
	if (strcmp(commands[0], "freeMft") == 0) {
		printf("free MFT: %d\n", getNumberOfFreeMft());
		return TRUE;
	}
	if (strcmp(commands[0], "printBit") == 0) {
		printBits(boot->cluster_count / 8, bitmap);
		return TRUE;
	}
	if (strcmp(commands[0], "printMft") == 0) {
		printList();
		return TRUE;
	}
	if (strcmp(commands[0], "cp") == 0) {
		Mft_Item *from;
		Resolut *res;
		//overim dostatek argumentu
		if (commands[1] == NULL || commands[2] == NULL) {
			printf("MISSING ARGUMENT (chybejici argument)\n");
			return FALSE;
		}
		//pokusim se najit zdroj
		if ((from = parsePath(commands[1], false)) == NULL) {
			printf("FILE NOT FOUND (neni zdroj)\n");
			return FALSE;
		}
		if ((res = destination(commands[2], true)) == NULL) {
			printf("PATH NOT FOUND (neexistuje cilova cesta)\n");
			return FALSE;
		}
		if (cp(from, res->item, res->name) == FALSE) {
			free(res);
			printf("FAIL\n");
			return FALSE;
		}

		printf("OK\n");
		free(res);
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
