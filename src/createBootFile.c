/*
 * createBootFile.c
 *
 *  Created on: 20.12.2017
 *      Author: jokertwo
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "boot_record.h"
#include "createBootFile.h"



int pocetMftItem = 10;

void *createTextFile(void *arg) {

	printf("Vytvarim testovaci soubor.\n");

	FILE *fp;

	if ((fp = fopen("Test01.bin", "wb")) == NULL) {
		//chyba pri otevirani souboru
		printf("Chyba pri otevirani souboru \n");
		return FALSE;
	}

	//vytvorim strukturu a naplnim ji daty
	boot_record *boot = malloc(sizeof(struct boot_record));

	strcpy(boot->signature, "Test01");
	strcpy(boot->volume_descriptor, "Prvni pokus o bootFile");
	boot->cluster_size = 1024;
	boot->cluster_count = 20;
	boot->disk_size = boot->cluster_size * boot->cluster_count;
	boot->mft_max_fragment_count = 5;
	boot->mft_start_address = 288;
	boot->bitmap_start_address = boot->mft_start_address + (sizeof(mft_item) * pocetMftItem);
	boot->data_start_address = boot->bitmap_start_address + (boot->cluster_count * sizeof(int8_t));

	//zapisu do souboru
	printf("Zapisuju do souboru\n");

	//zapisu boot
	fwrite(boot, sizeof(boot_record), 1, fp);

	//zapisu mft itemy
	int a = boot->bitmap_start_address - boot->mft_start_address;
	a = a / sizeof(mft_item);
	for (int i = 0; i < a; i++) {
		fseek(fp, boot->mft_start_address + (i * sizeof(mft_item)), SEEK_SET);
		mft_item *item = createItem(FREE_ITEM, 0, 1, 1, "0", 1);
		fwrite(item, sizeof(mft_item), 1, fp);
		free(item);
	}

	//zapisu bitmapu
	int8_t *bitmap = (int8_t*) calloc(boot->cluster_count, sizeof(int8_t));
	fseek(fp, boot->bitmap_start_address, SEEK_SET);
	fwrite(bitmap, boot->cluster_count * sizeof(int8_t), 1, fp);

	//uvolnim pamet
	free(bitmap);
	free(boot);
	//zavru soubor
	fclose(fp);


	return (int*) 1;
}
mft_item *createItem(int32_t UID, bool isDirectory, int8_t itemOrder,
		int8_t itemOrderTotal, char *name, int32_t itemSize) {
	mft_item *item = malloc(sizeof(struct mft_item));
	item->uid = UID;
	item->isDirectory = isDirectory;
	item->item_order = itemOrder;
	item->item_order_total = itemOrderTotal;
	memset(item->item_name, 0, sizeof(item->item_name));
	strcpy(item->item_name, name);
	item->item_size = itemSize;
	for (int i = 0; i < 32; i++) {
		item->fragments[i] = createFragment();
	}
	return item;
}
mft_fragment createFragment() {
	struct mft_fragment fragment;
	fragment.fragment_count = 1;
	fragment.fragment_start_address = 0;
	return fragment;
}



