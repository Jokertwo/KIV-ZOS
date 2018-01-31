/*
 * createBootFile.h
 *
 *  Created on: 20.12.2017
 *      Author: jokertwo
 */

#ifndef CREATEBOOTFILE_H_
#define CREATEBOOTFILE_H_
#include <stdbool.h>
#include "mft_item.h"
#include "globalVariable.h"


void *createTextFile(void *arg);
mft_fragment createFragment();
mft_item *createItem(int32_t UID, bool isDirectory, int8_t itemOrder,
		int8_t itemOrderTotal, char *name, int32_t itemSize);

#endif /* CREATEBOOTFILE_H_ */

