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
	//Mft_Item items[10];
	printf("Nacitam testovaci soubor.\n");
	//vytvorim strukturu a naplnim ji daty
	boot = calloc(sizeof(boot_record), 1);

	if ((fp = fopen("Test01.bin", "rb")) == NULL) {
		//chyba pri otevirani souboru
		printf("Chyba pri otevirani souboru \n");
		return 0;
	}
	fread(boot, sizeof(struct boot_record), 1, fp);

	//nactu mft tabulku
	//posunu ukazovatko na zacatek mft
	fseek(fp, boot->mft_start_address, SEEK_SET);
	//zjistim si pocet zaznamu
	int a = boot->bitmap_start_address - boot->mft_start_address;
	a = a / sizeof(Mft_Item);

	//postupne je pojednom nactu
	for (int i = 0; i < a; i++) {
		fseek(fp, boot->mft_start_address + (sizeof(Mft_Item) * i), SEEK_SET);
		Mft_Item *item = (Mft_Item *) malloc(sizeof(Mft_Item));
		fread(item, sizeof(Mft_Item), 1, fp);
		push(item);
	}

	//nactu bitmapu
	bitmap = calloc(boot->cluster_count / 8, sizeof(int8_t));
	//jdu na zacatek bitmapys
	fseek(fp, boot->bitmap_start_address, SEEK_SET);
	fread(bitmap, sizeof(int8_t), boot->cluster_count / 8, fp);

	return (int*) 1;
}

void clean() {
	//uvovlneni pameti
	clearList();
	free(bitmap);
	free(boot);
	fclose(fp);
}

