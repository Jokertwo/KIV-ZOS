/*
 * shellHelp.c
 *
 *  Created on: 4.2.2018
 *      Author: jokertwo
 */
#define MAX_DEEP 200
#include "shell.h"
Mft_Item *helpDirContains(Mft_Item *item, char *nameOfFile);
char** splitPath(char *path, int *numOfSplits);
void freePaths(int deep, char **paths);
int helpIsDirEmpty(Mft_Item *item);
int helpRemoveFromDir(Mft_Item *fromItem, int UID);

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
			if ((temp = dirContains(tempPosition, paths[i])) == NULL) {
				return NULL;
			}
			//pokud ano sestoupim niz
			tempPosition = temp;
		}
	}
	freePaths(count, paths);
	return tempPosition;
}

char** splitPath(char *path, int *numOfSplits) {
	char *token;
	int i = 0;
	char **paths;

	paths = malloc(boot->cluster_count);
	token = strtok(path, "/");
	while (token != NULL) {
		paths[i] = calloc(strlen(token) + 1, sizeof(char));
		strncpy(paths[i], token, strlen(token));
		i++;
		token = (strtok(NULL, "/"));
	}
	*numOfSplits = i;
	return paths;
}
/**
 * Zkontroluje jestli predana slozka obsahuje
 * Adresar/soubor se jmenem predanym jako druhy argument
 */
Mft_Item *dirContains(Mft_Item *dir, char *nameOfFile) {
	Mft_Item *temp;
	Mft_Item *resolut = NULL;
	int i = 1;
	temp = dir;
	while (temp != NULL) {
		if ((resolut = helpDirContains(temp, nameOfFile)) != NULL) {
			return resolut;
		}
		i++;
		temp = getMftItemByUID(dir->uid, i);
	}
	return NULL;
}
/**
 * pomocna metoda k dirContains
 */
Mft_Item *helpDirContains(Mft_Item *item, char *nameOfFile) {
	char *content;
	char *tempUID;
	Mft_Item *temp;

	for (int i = 0; i < MAX_FRAGMENT_COUNT; i++) {
		if (item->fragments[i].fragment_start_address == VOID) {
			continue;
		}
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
/**
 * zjisti jestli je slozka prazdna
 */
int isDirEmpty(Mft_Item *dir) {
	Mft_Item *temp;
	temp = dir;
	int i = 1;
	while (temp != NULL) {
		if (helpIsDirEmpty(temp) == FALSE) {
			return FALSE;
		}
		i++;
		temp = getMftItemByUID(temp->uid, i);
	}
	return TRUE;
}
int helpIsDirEmpty(Mft_Item *item) {
	char *content;
	char *tempUID;
	Mft_Item *temp;
	for (int i = 0; i < MAX_FRAGMENT_COUNT; i++) {
		if (item->fragments[i].fragment_start_address == VOID) {
			continue;
		}
		content = getClusterContent(item->fragments[i].fragment_start_address,
				item->fragments[i].fragment_count);
		//projdu jednotliva UID a hledam shodu
		tempUID = strtok(content, DELIMETER);
		while (tempUID != NULL) {
			//pokud jsem neco v clusteru nasel pokusim se tomu priradit zaznam v mft
			//v opacnem pripade je jedna jen o sum
			temp = getMftItemByUID(atoi(tempUID), 1);
			if (temp != NULL) {
				free(content);
				content = NULL;
				return FALSE;
			}
			tempUID = strtok(NULL, DELIMETER);
		}
		free(content);
		content = NULL;
	}
	return TRUE;
}
int removeDir(Mft_Item *itemToRemove, Mft_Item *fromDir) {
	Mft_Item *temp;
	temp = fromDir;
	int i = 1;

	while (temp != NULL) {
		if (helpRemoveFromDir(temp, itemToRemove->uid) == TRUE) {
			return TRUE;
		}
		i++;
		temp = getMftItemByUID(temp->uid, i);
	}
	return FALSE;
}
int helpRemoveFromDir(Mft_Item *fromItem, int UID) {
	int end = FALSE;
	char *newContent;
	char *content;
	char *tempUID;
	char stringUID[12];
	for (int i = 0; i < MAX_FRAGMENT_COUNT; i++) {
		if (fromItem->fragments[i].fragment_start_address == VOID) {
			continue;
		}
		newContent = calloc(boot->cluster_size,
				fromItem->fragments[i].fragment_count);
		content = getClusterContent(
				fromItem->fragments[i].fragment_start_address,
				fromItem->fragments[i].fragment_count);
		tempUID = strtok(content, DELIMETER);
		while (tempUID != NULL) {
			if (atoi(tempUID) == UID) {
				end = TRUE;
			} else {
				memset(stringUID, 0, 12);
				sprintf(stringUID, "%s%s", tempUID, DELIMETER);
				strcat(newContent, stringUID);
			}
			tempUID = strtok(NULL, DELIMETER);
		}
		if (end == TRUE) {
			fseek(fp, fromItem->fragments[i].fragment_start_address, SEEK_SET);
			fwrite(newContent,
					boot->cluster_size * fromItem->fragments[i].fragment_count,
					1, fp);
			free(content);
			free(newContent);
			content = NULL;
			newContent = NULL;
			return TRUE;
		}
		free(content);
		free(newContent);
		content = NULL;
		newContent = NULL;
	}
	return FALSE;
}

void nullMftItem(Mft_Item *item) {
	item->uid = FREE_ITEM;
	item->backUid = 0;
	memset(item->item_name, 0, 12);
	item->item_order = 0;
	item->item_order_total = 0;
	item->item_size = 0;
	for (int i = 0; i < MAX_FRAGMENT_COUNT; i++) {
		item->fragments[i].fragment_count = 0;
		item->fragments[i].fragment_start_address = VOID;
	}
}
void writeChangeToFile() {
	writeToFile(boot->bitmap_start_address, boot->cluster_count);
	//zapisu mft do souboru
	writeMftToFile();
}

void freePaths(int deep, char **paths) {
	for (int i = 0; i < deep; i++) {
		free(paths[i]);
		paths[i] = NULL;
	}
	free(paths);
}

