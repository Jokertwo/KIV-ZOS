/*
 * shellHelp.c
 *
 *  Created on: 4.2.2018
 *      Author: jokertwo
 */
#define MAX_DEEP 200
#include "shell.h"
Mft_Item *helpDirContains(Mft_Item *item, char *nameOfFile, bool isDir);
char** splitPath(char *path, int *numOfSplits);
void freePaths(int deep, char **paths);
int helpIsDirEmpty(Mft_Item *item);
int helpRemoveFromDir(Mft_Item *fromItem, int UID, FILE *fp);

/**
 * Postupne prochazim zadanou cestou a zkousim jestli existuje nebo ne
 */
Mft_Item *parsePath(char *path, bool isDir) {

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
			if ((temp = dirContains(tempPosition, paths[i],  i == count-1 ? isDir : true)) == NULL) {
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
Mft_Item *dirContains(Mft_Item *dir, char *nameOfFile, bool isDir) {
	Mft_Item *temp;
	Mft_Item *resolut = NULL;
	int i = 1;
	temp = dir;
	while (temp != NULL) {
		if ((resolut = helpDirContains(temp, nameOfFile, isDir)) != NULL) {
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
Mft_Item *helpDirContains(Mft_Item *item, char *nameOfFile, bool isDir) {
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
			if (strcmp(temp->item_name, nameOfFile) == 0 && isDir
					== temp->isDirectory) {
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
Resolut *destination(char *path, bool isDir) {

	char *tempPath;
	int size;
	Resolut *res = calloc(sizeof(Resolut), 1);
	//hledam jestli byla zadana cesta do slozky
	if ((res->name = strrchr(path, '/')) != NULL) {
		//odriznu si cestu
		size = res->name - path;
		if (size > 0) {
			//alokuju si pamet pro cestu(o jedna vetsi pro ukoncujici znak)
			tempPath = calloc(size + 1, sizeof(char));
			//vyjmu si cestu z argumentu
			strncpy(tempPath, path, size);
		} else {
			tempPath = calloc(2, sizeof(char));
			strcpy(tempPath, "/");
		}
		//cestu si projdu a zjistim tak jestli existuje
		if ((res->item = parsePath(tempPath, isDir)) == NULL) {
			free(tempPath);
			free(res);
			return NULL;
		}
		free(tempPath);
		//odstranim si z nazvu lomitko
		res->name++;

	}
	//nebo vytvorim slozku tam kde jsem
	else {
		res->item = position;
		res->name = path;
	}
	return res;
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
int removeFromDir(Mft_Item *itemToRemove, Mft_Item *fromDir) {
	Mft_Item *temp;
	temp = fromDir;
	int i = 1;
	FILE *fp;
	if ((fp = fopen(fileName, "r+")) == NULL) {
		debugs("removeFromDir: napovedlo se otevrit soubor pro zapis");
	}
	while (temp != NULL) {
		if (helpRemoveFromDir(temp, itemToRemove->uid, fp) == TRUE) {
			fclose(fp);
			return TRUE;
		}
		i++;
		temp = getMftItemByUID(temp->uid, i);
	}
	fclose(fp);
	return FALSE;
}
int helpRemoveFromDir(Mft_Item *fromItem, int UID, FILE *fp) {
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
char *bufferForCp(FILE *fp, Mft_Item *item) {
	char *buffer;
	buffer = calloc(item->item_size + 1, sizeof(char));
	char *tempBuffer;
	for (int i = 1; i <= item->item_order_total; i++) {
		for (int j = 0; j < MAX_FRAGMENT_COUNT; j++) {
			if (item->fragments[j].fragment_start_address != VOID) {
				tempBuffer = calloc(boot->cluster_size,
						item->fragments[j].fragment_count);
				fseek(fp, item->fragments[j].fragment_start_address, SEEK_SET);
				fread(tempBuffer, boot->cluster_size,
						item->fragments[j].fragment_count, fp);
				strcat(buffer, tempBuffer);
				free(tempBuffer);
			}
		}
		if (i < item->item_order_total) {
			item = getMftItemByUID(item->uid, i + 1);
		}
	}
	return buffer;
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
	countFreeMft(item);
}
int updateSize(Mft_Item *startItem, bool upOrDown) {
	Mft_Item *temp;
	int size;
	if (startItem->backUid == ROOT_BACK_UID) {
		debugs("updateSize: Argumentem nemuze byt root");
		return FALSE;
	}
	size = startItem->item_size;
	temp = getMftItemByUID(startItem->backUid, 1);

	while (temp->backUid != ROOT_BACK_UID) {
		temp->item_size = upOrDown ? temp->item_size + size : temp->item_size
				- size;
		startItem = temp;
		temp = getMftItemByUID(temp->backUid, 1);
	}
	//upravim velikost u rootu
	temp->item_size = upOrDown ? temp->item_size + size : temp->item_size
			- size;
	return TRUE;

}
void writeChangeToFile() {
	writeToFile(boot->bitmap_start_address, boot->cluster_count);
	//zapisu mft do souboru
	writeMftToFile();
}
/**
 * vztiskne info o jednom mft_Itemu
 */
void printInfoAboutMftItem(Mft_Item *item) {
	printf("%s - %d - %d \n", item->item_name, item->uid, item->item_size);
}

void freePaths(int deep, char **paths) {
	for (int i = 0; i < deep; i++) {
		free(paths[i]);
		paths[i] = NULL;
	}
	free(paths);
}
/**
 * zjisti pocet bloku
 */
int getNumberOfBitBlocks(int *bits, int sizeOfArray) {
	int *tempBits = bits;
	int count = 0;
	int index = 0;
	int size;
	while (index < sizeOfArray) {
		count++;
		size = getSizeOfBitBlock(tempBits, sizeOfArray - index);
		index += size;
		if (index < sizeOfArray) {
			tempBits = tempBits + size;
		}
	}
	return count;
}

void afterCp(int numberOfClusters, int *bits, Mft_Item *new, Mft_Item *parent) {
	//zapisu zmeny do bitmapy
	for (int i = 0; i < numberOfClusters; i++) {
		writeBit(*(bits + i));
	}

	free(bits);
	//zapisu soubor do nadrazene slozky
	char *sUID = calloc(intLeng(new->uid) + 2, sizeof(char));
	sprintf(sUID, "%d#", new->uid);
	addToCluster(sUID, parent->fragments[0].fragment_start_address);
	free(sUID);
	//updatuji velikost u nadrazenych slozek
	updateSize(new, true);
	//zapisu bitmapu a mft do souboru
	writeChangeToFile();
}
/**
 * zjisti kolik bitu jde postupne zasebou
 */
int getSizeOfBitBlock(int *bits, int sizeOfArray) {
	int *temp = bits;
	for (int i = 0; i < (sizeOfArray - 1); i++) {
		if (*(temp) != (*(temp + 1) - 1)) {
			return i + 1;
		}
		temp++;
	}
	return sizeOfArray;
}
/**
 * vraci pocet potrebnych clusteru pro ulozeni souboru
 * o zadane velikosti
 */
int getNumberOfClusters(int fileSize) {
	if ((fileSize % boot->cluster_size) == 0)
		return fileSize / boot->cluster_size;
	return (fileSize / boot->cluster_size) + 1;
}
int intLeng(int n) {
	if (n < 10)
		return 1;
	return 1 + intLeng(n / 10);
}
