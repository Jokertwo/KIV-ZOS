/*
 * defrag.c
 *
 *  Created on: 8.2.2018
 *      Author: jokertwo
 */

#include "shell.h"
void *find(void *arg);
Mft_Item *getMftByAdress(int adress);
int defrag(void);

Mft_Item *getMftByAdress(int adress) {

	Mft_Item *item = NULL;

	pthread_t threads[3];
	initConsistency();
	while (isNext() == true) {
		for (int i = 0; i < 3; i++) {
			pthread_create(&threads[i], NULL, find, (void *) adress);
		}
		for (int i = 0; i < 3; i++) {
			void *temp;
			pthread_join(threads[i], &temp);
			if (temp != NULL) {
				item = (Mft_Item *) temp;
				break;
			}
		}
	}
	finishConsistency();
	return item;

}

void *find(void *arg) {
	int adress = (int) arg;

	Mft_Item *item;
	if ((item = getNext()) != NULL) {
		for (int i = 0; i < MAX_FRAGMENT_COUNT; i++) {
			if (item->fragments[i].fragment_start_address == adress) {
				return item;
			}
		}
	}
	return NULL;
}

int defrag(void) {
	Mft_Item *item;
	Mft_Item *last;
	int *newBits;
	FILE *fp;
	initConsistency();
	char *buffer;
	char *tempBuffer;

	while ((item = getNext()) != NULL) {
		if (item->item_order != 1) {
			continue;
		}
		if (item->uid == ROOT) {
			continue;
		}
		if ((newBits = getFreeBlocBits(getNumberOfClusters(item->item_size),
				boot->cluster_count)) == NULL) {
			continue;
		}
		for (int i = 0; i < getNumberOfClusters(item->item_size); i++) {
			printf("%d\n", *(newBits + i));
		}

		if ((fp = fopen(fileName, "r+")) == NULL) {
			debugs("defrag: Nepovedlo se otevrit soubor pro zapis.");
			free(newBits);
			continue;
		}
		last = item;
		buffer = malloc((item->item_size * sizeof(char)) + 1);
		memset(buffer, 0, (item->item_size * sizeof(char)) + 1);
		for (int i = 1; i <= item->item_order_total; i++) {
			for (int j = 0; j < MAX_FRAGMENT_COUNT; j++) {
				if (item->fragments[j].fragment_start_address != VOID) {
					int temp = item->fragments[j].fragment_start_address
							- boot->data_start_address;
					temp = temp / boot->cluster_size;
					temp++;
					for (int h = 0; h < item->fragments[j].fragment_count; h++) {
						deleteBit(temp + h);
					}
					tempBuffer = malloc(
							(item->fragments[j].fragment_count
									* boot->cluster_size) + 1);
					memset(
							tempBuffer,
							0,
							item->fragments[j].fragment_count
									* boot->cluster_size);
					fseek(fp, item->fragments[j].fragment_start_address,
							SEEK_SET);
					fread(tempBuffer, boot->cluster_size,
							item->fragments[j].fragment_count, fp);
					strcat(buffer, tempBuffer);
					fflush(fp);
					free(tempBuffer);
				}
			}
			int pom = item->item_order_total;
			int uid = item->uid;
			if (item->item_order != 1) {
				nullMftItem(item);
			}
			if (i < pom) {
				item = getMftItemByUID(uid, i + 1);
			}
		}
		last->item_order_total = 1;
		last->fragments[0].fragment_start_address = ((*newBits - 1)
				* boot->cluster_size) + boot->data_start_address;
		last->fragments[0].fragment_count
				= getNumberOfClusters(last->item_size);
		fseek(fp, last->fragments[0].fragment_start_address, SEEK_SET);
		fwrite(buffer, boot->cluster_size, last->fragments[0].fragment_count,
				fp);
		for (int i = 0; i < last->fragments[0].fragment_count; i++) {
			writeBit(*(newBits + i));
		}
		fclose(fp);
		free(newBits);
		free(buffer);
		writeChangeToFile();

	}
	finishConsistency();
	return true;
}
