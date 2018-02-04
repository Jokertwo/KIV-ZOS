/*
 * createBootFile.h
 *
 *  Created on: 20.12.2017
 *      Author: jokertwo
 */

#ifndef CREATEBOOTFILE_H_GUARD
#define CREATEBOOTFILE_H_GUARD

#include "mft_item.h"
#include "globalVariable.h"
#include "bitMap.h"

void *createTextFile(void *arg);
void createFragment(Mft_Item *item, int i);
Mft_Item *createItem(int32_t UID, bool isDirectory, int8_t itemOrder,
		int8_t itemOrderTotal, char *name, int32_t itemSize,int32_t back);

#endif /* CREATEBOOTFILE_H_ */

