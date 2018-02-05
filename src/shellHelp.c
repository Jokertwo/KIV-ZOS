/*
 * shellHelp.c
 *
 *  Created on: 4.2.2018
 *      Author: jokertwo
 */
#define MAX_DEEP 200
#include "shell.h"
Mft_Item *helpParse(Mft_Item *item, char *nameOfFile);
char** splitPath(char *path, int *numOfSplits);
void freePaths(int deep,char **paths);

/**
 * Postupne prochazim zadanou cestou a zkousim jestli existuje nebo ne
 */
Mft_Item *parsePath(char *path) {

	Mft_Item *tempPosition;
	Mft_Item *temp;
	int count = 0;
	char **paths;

	//rozdelim si cestu na jednotlive nazvy
	paths = splitPath(path, &count);

	//hledam od korene
	if (path[0] == '/') {
		tempPosition = root;
	}
	//hledam z aktualni pozice
	else {
		tempPosition = position;
	}

	for (int i = 0; i < count; i++) {
		//jdu o slozku vys
		if (strcmp(paths[i], "..") == 0) {
			temp = getMftItemByUID(tempPosition->backUid, 1);
			if (temp == NULL) {
				debugs("parsePath: Chyba v backUID:'%d' behem parsovani cesty %s",tempPosition->backUid,path);

				return NULL;
			}
			tempPosition = temp;
		}
		//hledam jestli slozka ve ktere jsem obsahuje tu kterou hledam
		else {
			//pokud ne vracim NULL cesta je spatne
			if ((temp = helpParse(tempPosition, paths[i])) == NULL) {
				return NULL;
			}
			//pokud ano sestoupim niz
			tempPosition = temp;
		}
	}
	freePaths(count,paths);
	return tempPosition;
}

char** splitPath(char *path, int *numOfSplits) {
	char *token;
	int i = 0;
	char **paths;

	paths = malloc(boot->cluster_count);
	token = strtok(path, "/");
	while (token != NULL) {
		paths[i] = calloc(strlen(token) + 1,sizeof(char));
		strncpy(paths[i], token, strlen(token));
		i++;
		token = (strtok(NULL, "/"));
	}
	*numOfSplits = i;
	return paths;
}
Mft_Item *helpParse(Mft_Item *item, char *nameOfFile) {
	char *content;
	char *tempUID;
	Mft_Item *temp;

	for (int i = 0; i < MAX_FRAGMENT_COUNT; i++) {
		content = getClusterContent(item->fragments[i].fragment_start_address,
				item->fragments[i].fragment_count);
		//projdu jednotliva UID a hledam shodu
		tempUID = strtok(content, DELIMETER);
		while (tempUID != NULL) {
			temp = getMftItemByUID(atoi(tempUID), 1);
			if (temp == NULL) {
				debugs("parsePath: Slozka s nazvem %s ma zaznam v clusteru ze ma obsahovat \nslozku/soubor o UID:%s ale nenasel jsem odpovidajici mft_item",item->item_name,tempUID);
			}
			if (strcmp(temp->item_name, nameOfFile) == 0) {
				free(content);
				content = NULL;
				return temp;
			}
			tempUID = strtok(NULL, DELIMETER);
		}
		free(content);
		content = NULL;
	}

	return NULL;
}

void freePaths(int deep,char **paths) {
	for (int i = 0; i < deep; i++) {
		free(paths[i]);
		paths[i] = NULL;
	}
	free(paths);
}



