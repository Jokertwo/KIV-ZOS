/*
 * mftHelper.h
 *
 *  Created on: 1.2.2018
 *      Author: jokertwo
 */

#ifndef MFTHELPER_H_GUARD
#define MFTHELPER_H_GUARD


#include "mft_item.h"
#include <pthread.h>

pthread_rwlock_t list_lock;

void printList(void);
void clearList(void);
bool isEmpty();
void push(Mft_Item *item);
Mft_Item *getMftItemByUID(int32_t UID,int8_t itemOrder);
Mft_Item *getMftItemByName(char *itemName,int8_t itemOrder);
Mft_Item *getFreeMftItem();
int getNewUID(void);
void writeMftToFile(void);

#endif /* MFTHELPER_H_ */
