/*
 * shellHelp.c
 *
 *  Created on: 4.2.2018
 *      Author: jokertwo
 */

#include "shell.h"

Mft_Item *parsePath(char *path) {
	char *name;
	Mft_Item *tempPosition;
	Mft_Item *temp;
	if (path[0] == '/') {
		tempPosition = root;
	} else {
		tempPosition = position;
	}

	name = strtok(path, "/");
	while (name != NULL) {
		if (strcmp(name, "..") == 0) {
			temp = getMftItemByUID(tempPosition->backUid, 1);
			if (temp == NULL) {
				debugs("parsePath: Chyba v backUID behem parsovani cesty %s",path);
				return NULL;
			}
			tempPosition = temp;
		} else {
			temp = getMftItemByName(name, 1);
			if (temp == NULL) {
				debugs("parsePath: Slozka s nazvem %s neobsahuje slozku/soubor %s",tempPosition->item_name,name);
				return NULL;
			}
			tempPosition = temp;
		}
		name = strtok(NULL,"/");
	}
	return tempPosition;
}
