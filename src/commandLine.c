/*
 * commandLine.c
 *
 *  Created on: 3.2.2018
 *      Author: jokertwo
 */
#define MAX 200
#define END -1000
#include "commandLine.h"

int functions(char **commands);
void printHelp(void);
void freeCommands(int numCommands, char **commands);
void prepareCommands(char **commands);
void splitCommand(char **commands, char *inputLine, int *numCommand);

void prepareCommands(char **commands) {
	for (int i = 0; i < MAX; i++) {
		commands[i] = NULL;
	}
}

void freeCommands(int numCommands, char **commands) {
	for (int i = 0; i < numCommands; i++) {
		free(commands[i]);
		commands[i] = NULL;
	}
}
void splitCommand(char **commands, char *inputLine, int *numCommand) {
	char *input;
	input = strtok(inputLine, " ");
	for (int i = 0; input != NULL; i++) {
		*numCommand = *numCommand + 1;
		commands[i] = malloc(strlen(input) + 1);
		memset(commands[i], 0, strlen(input) + 1);
		strncpy(commands[i], input, strlen(input));
		input = strtok(NULL, " ");
	}
}

void *commandLine(void *args) {

	//buffer pro prikaz
	char* inputLine = (char*) malloc(MAX);
	int numCommand = 0;
	char* commands[MAX];

	prepareCommands(commands);
	while (true) {
		memset(inputLine, 0, MAX);
		pwd();
		printf(":>");

		fgets(inputLine, MAX, stdin);
		inputLine[strlen(inputLine) - 1] = '\0';

		if (strlen(inputLine) == 0) {
			continue;
		}
		splitCommand(commands, inputLine, &numCommand);

		if (functions(commands) == END) {
			freeCommands(numCommand, commands);
			break;
		}
		freeCommands(numCommand, commands);
		numCommand = 0;

	}
	free(inputLine);
	return (int*) TRUE;
}

int functions(char **commands) {

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
	if (strcmp(commands[0], "mv") == 0) {
		Mft_Item *s1;
		Resolut *res;
		if (commands[1] == NULL || commands[2] == NULL) {
			printf("MISSING ARGUMENT (chybejici argument)\n");
			return FALSE;
		}
		//pokusim se najit zdroj
		if ((s1 = parsePath(commands[1], false)) == NULL) {
			printf("FILE NOT FOUND (neni zdroj)\n");
			return FALSE;
		}
		if ((res = destination(commands[2], true)) == NULL) {
			printf("PATH NOT FOUND (neexistuje cilova cesta)\n");
			return FALSE;
		}
		if (mv(s1, res->item, res->name) == FALSE) {
			printf("FAIL\n");
			free(res);
			return FALSE;
		}
		free(res);
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
	if(strcmp(commands[0], "conCheck") == 0){
		conCheck(3);
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
		printf("OK\n");
		return TRUE;
	}
	if (strcmp(commands[0], "outcp") == 0) {
		Mft_Item *from;
		FILE *file,*fp;
		char *buffer;
		//overim dostatek argumentu
		if (commands[1] == NULL || commands[2] == NULL) {
			printf("MISSING ARGUMENT (chybejici argument)\n");
			return FALSE;
		}
		if((from = parsePath(commands[1],false)) == NULL){
			printf("FILE NOT FOUND (neni zdroj)\n");
			return FALSE;
		}
		if((file = fopen(commands[2], "w")) == NULL){
			printf("PATH NOT FOUND (neexistujici cilova cesta)\n");
			return FALSE;
		}
		//otevru svuj souborovy system a vyjmu si obsah
		if((fp = fopen(fileName,"r")) == NULL){
			printf("FAIL NTFS\n");
			fclose(file);
			return FALSE;
		}
		buffer = bufferForCp(fp,from);
		fwrite(buffer,from->item_size,1,file);
		fclose(file);
		fclose(fp);
		printf("OK\n");
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
		//a cilovou slozku
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
	if (strcmp(commands[0], "load") == 0) {
		int numCommandFromFile = 0;
		char line[MAX_LINE];
		char* commandFromFile[MAX];
		FILE *file;
		if (commands[1] == NULL) {
			printf("MISSING ARGUMENT (chybejici argument)\n");
			return FALSE;
		}
		if ((file = fopen(commands[1], "r")) == NULL) {
			printf("FILE NOT FOUND (neni zdroj)");
			return false;
		}
		prepareCommands(commandFromFile);
		//budu cist po jedne radce
		while (fgets(line, MAX_LINE, file) != NULL) {
			int delka = strlen(line);
			//pokud jsem neprecetl celou radku nemuyu ji ypracovat
			if (delka == MAX_LINE - 1 && line[delka - 1] != '\n') {
				printf("Chyba - vstupni raka prilis dlouha \n %s \n", line);
				continue;
			}
			if (line[delka - 1] == '\n') {
				line[delka - 1] = '\0';
			}
			//predpracuju si jeden prikaz
			splitCommand(commandFromFile, line, &numCommandFromFile);
			//a ypracuju ho
			if (functions(commandFromFile) == END) {
				freeCommands(numCommandFromFile, commandFromFile);
				printf("OK\n");
				fclose(file);
				return END;
			}
			freeCommands(numCommandFromFile, commandFromFile);
			numCommandFromFile = 0;
		}
		fclose(file);
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
