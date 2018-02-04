/*
 * mft_item.h
 *
 *  Created on: 20.12.2017
 *      Author: jokertwo
 */
#ifndef MFT_ITEM_H_GUARD
#define MFT_ITEM_H_GUARD
#define MAX_FRAGMENT_COUNT 5

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "boot_record.h"
#include "mft_fragments.h"

typedef struct mft_item {
	int32_t uid; //UID polozky, pokud UID = UID_ITEM_FREE, je polozka volna
	bool isDirectory; //soubor, nebo adresar
	int8_t item_order; //poradi v MFT pri vice souborech, jinak 1
	int8_t item_order_total; //celkovy pocet polozek v MFT
	char item_name[12]; //8+3 + /0 C/C++ ukoncovaci string znak
	int32_t item_size; //velikost souboru v bytech
	int32_t backUid;
	struct mft_fragment fragments[32]; //fragmenty souboru
}Mft_Item;

typedef struct mft_list{
	Mft_Item *item;
	struct mft_list *next;
}MFT_List;

Mft_Item *position;


#endif /* MFT_ITEM_H_ */
