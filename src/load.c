/*
 * load.c
 *
 *  Created on: 29.1.2018
 *      Author: jokertwo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "load.h"
#include "bitMap.h"

void *readBoot(void *arg) {
	//mft_item items[10];
	printf("Nacitam testovaci soubor.\n");
	//vytvorim strukturu a naplnim ji daty
	boot_record *boot = malloc(sizeof(struct boot_record));

	FILE *fp;
	if ((fp = fopen("Test01.bin", "rb")) == NULL) {
		//chyba pri otevirani souboru
		printf("Chyba pri otevirani souboru \n");
		return 0;
	}
	fread(boot, sizeof(struct boot_record), 1, fp);
	fseek(fp, boot->mft_start_address, SEEK_SET);

	int a = boot->bitmap_start_address - boot->mft_start_address;
	a = a / sizeof(mft_item);
	for (int i = 0; i < a; i++) {
		fseek(fp, boot->mft_start_address + (sizeof(mft_item) * i), SEEK_SET);
		mft_item *item = malloc(sizeof(mft_item));
		fread(item, sizeof(struct mft_item), 1, fp);
		printMftItem(item);
		free(item);
	}
	bitmap = (int8_t*) calloc(boot->cluster_count, sizeof(int8_t));
	fseek(fp, boot->bitmap_start_address, SEEK_SET);
	fread(bitmap,boot->cluster_count * sizeof(int8_t),1,fp);
	printBits(boot->cluster_count);

	fclose(fp);
	free(boot);

	return (int*) 1;
}

void printMftItem(mft_item *item) {
	printf("UID %d\n", item->uid);
	printf("isDirectory %d\n", item->isDirectory);
	printf("itemOrder %d\n", item->item_order);
	printf("itemName %s\n", item->item_name);
}
