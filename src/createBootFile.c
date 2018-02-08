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
	boot_record *boot;

	if ((fp = fopen(fileName, "wb")) == NULL) {
		//chyba pri otevirani souboru
		printf("Chyba pri otevirani souboru \n");
		return (int*) FALSE;
	}

	//vytvorim strukturu a naplnim ji daty
	boot = calloc(sizeof(boot_record),1);

	strcpy(boot->signature, "Test01");
	strcpy(boot->volume_descriptor, "Prvni pokus o bootFile");
	boot->cluster_size = 1024;
	boot->cluster_count = 32;
	boot->disk_size = boot->cluster_size * boot->cluster_count;
	boot->mft_max_fragment_count = MAX_FRAGMENT_COUNT;
	boot->mft_start_address = 288;
	boot->bitmap_start_address = boot->mft_start_address + (sizeof(Mft_Item)
			* pocetMftItem);
	boot->data_start_address = boot->bitmap_start_address
			+ (boot->cluster_count * sizeof(int8_t));

	//zapisu do souboru
	printf("Zapisuju do souboru\n");

	//zapisu boot
	fwrite(boot, sizeof(boot_record), 1, fp);

	//zapisu mft itemy
	int a = boot->bitmap_start_address - boot->mft_start_address;
	a = a / sizeof(Mft_Item);
	for (int i = 1; i < a; i++) {
		fseek(fp, boot->mft_start_address + (i * sizeof(Mft_Item)), SEEK_SET);
		Mft_Item *item = createItem(FREE_ITEM, 0, 0, 0, "0", 0, 0);
		fwrite(item, sizeof(Mft_Item), 1, fp);
		free(item);
	}

	//zapisu bitmapu
	bitmap = calloc(boot->cluster_count / 8, sizeof(int8_t));
	fseek(fp, boot->bitmap_start_address, SEEK_SET);
	fwrite(bitmap, sizeof(int8_t), boot->cluster_count / 8, fp);

	////////////////////// korenovy adresar//////////////////

	//vytvorim a zapisu korenovy adresar
	Mft_Item *root = createItem(ROOT, 1, 1, 1, "ROOT", 1, ROOT_BACK_UID);
	root->fragments[0].fragment_count = 1;
	root->fragments[0].fragment_start_address = boot->data_start_address;

	//zapisu do souboru mft zaznam
	fseek(fp, boot->mft_start_address, SEEK_SET);
	fwrite(root, sizeof(Mft_Item), 1, fp);

	//zapisu do bitmapy
	writeBit(1);
	fseek(fp, boot->bitmap_start_address, SEEK_SET);
	fwrite(bitmap, sizeof(int8_t), boot->cluster_count / 8, fp);

	//zapisu clustery
	char *cluster = calloc(boot->cluster_size, sizeof(char));
	for (int i = 0; i < boot->cluster_count; i++) {
		fseek(fp, boot->data_start_address + (i * boot->cluster_size), SEEK_SET);
		fwrite(cluster, boot->cluster_size, 1, fp);
	}

	//uvolnim pamet
	free(bitmap);
	free(boot);
	free(root);
	free(cluster);

	//zavru soubor
	fclose(fp);

	return (int*) 1;
}

Mft_Item *createItem(int32_t UID, bool isDirectory, int8_t itemOrder,
		int8_t itemOrderTotal, char *name, int32_t itemSize, int32_t back) {
	Mft_Item *item;
	item = calloc(sizeof(Mft_Item),1);
	item->uid = UID;
	item->isDirectory = isDirectory;
	item->item_order = itemOrder;
	item->item_order_total = itemOrderTotal;
	memset(item->item_name, 0, sizeof(item->item_name));
	strcpy(item->item_name, name);
	item->item_size = itemSize;
	item->backUid = back;
	for (int i = 0; i < MAX_FRAGMENT_COUNT; i++) {
		createFragment(item, i);
	}
	return item;
}
void createFragment(Mft_Item *item, int i) {

	item->fragments[i].fragment_count = 0;
	item->fragments[i].fragment_start_address = VOID;

}

