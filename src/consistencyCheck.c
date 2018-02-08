/*
 * consistentCheck.c
 *
 *  Created on: 8.2.2018
 *      Author: jokertwo
 */

#include "shell.h"

void printResolt(Mft_Item *item, int size);

int conCheck(int numberOfThread) {
	int value = 0;
	pthread_t threads[3];
	initConsistency();
	while (isNext() == true) {
		for (int i = 0; i < 3; i++) {
			pthread_create(&threads[i], NULL, checkConsistency, NULL);
		}
		for (int i = 0; i < 3; i++) {
			void *size;
			pthread_join(threads[i], &size);
			value += (int) size;
		}
	}
	finishConsistency();
	printf("Vysledek %d", value);
	return value;
}

void *checkConsistency(void *arg) {
	//initConsistency();
	Mft_Item *item;
	if ((item = getNext()) != NULL) {
		int size = 0;
		if (item->isDirectory == false) {

			//projdu vsechny mft fragmenty
			for (int i = 1; i <= item->item_order_total; i++) {
				for (int j = 0; j < MAX_FRAGMENT_COUNT; j++) {
					char *temp = getClusterContent(
							item->fragments[i].fragment_start_address,
							item->fragments[i].fragment_count);
					size += strlen(temp);
					free(temp);
				}
				if (i < item->item_order_total) {
					item = getMftItemByUID(item->uid, i);
				}
			}
			printResolt(item, size);
		} else {
			Mft_Item *tempMft;
			char *pom;
			char *temp = getClusterContent(
					item->fragments[0].fragment_start_address,
					item->fragments[0].fragment_count);
			size += strlen(temp);
			pom = strtok(temp, DELIMETER);
			while (pom != NULL) {
				tempMft = getMftItemByUID(atoi(pom), 1);
				size += tempMft->item_size;
				pom = strtok(NULL, DELIMETER);
			}
			free(temp);
			printResolt(item, size);
		}
		return (void*) size;
	}
	return (void*) 0;
}

void printResolt(Mft_Item *item, int size) {
	if (item->item_size != size) {
		printf(
				"Nekozistetni stav. %s %s ma ocekavanou velikost %d a realnou velikost %d\n",
				item->isDirectory ? "Slozka" : "Soubor", item->item_name,
				item->item_size, size);
		return;
	}
	printf("Kozistetni stav.   %s %s ma ocekavanou velikost %d a realnou velikost %d\n",
				item->isDirectory ? "Slozka" : "Soubor", item->item_name,
				item->item_size, size);
}
