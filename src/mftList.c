/*
 * mftHelper.c
 *
 *  Created on: 1.2.2018
 *      Author: jokertwo
 */

#include "mftList.h"
#include <stdlib.h>
#include <stdio.h>

void printMftItem(Mft_Item *item);
void deleteFirst(void);
void relaseMemory(MFT_List *item);


MFT_List *head = NULL;

/**
 * vlozi zaznam do listu
 */
void push(Mft_Item *item) {
	MFT_List *new = (MFT_List *) malloc(sizeof(MFT_List));
	new->item = item;

	pthread_rwlock_wrlock(&list_lock);
	new->next = head;
	head = new;
	pthread_rwlock_unlock(&list_lock);
}

/**
 * vytiskne cely list
 */
void printList() {
	MFT_List *temp = head;

	pthread_rwlock_rdlock(&list_lock);
	while (temp != NULL) {
		printMftItem(temp->item);
		temp = temp->next;
	}
	pthread_rwlock_unlock(&list_lock);

}
/**
 * Vymaze cely seznam
 */
void clearList() {
	while (!isEmpty()) {
		deleteFirst();
	}
}

/**
 * Rekne jestli je list prazdny nebo ne
 */
bool isEmpty() {
	if (head != NULL) {
		return false;
	}
	return true;
}
/**
 * Najde zaznam podle UID
 */
Mft_Item *getMftItemByUID(int32_t UID, int8_t itemOrder) {
	MFT_List *temp = head;
	while (temp != NULL) {
		if (temp->item->uid == UID && temp->item->item_order) {
			debugs("Nasel jsem zaznam v tabulce pro UID = %d\n",UID);
			return temp->item;
		}
		temp = temp->next;
	}
	debugs("V tabulce mft neexistuje zaznam s UID = %d\n",UID);
	return NULL;
}

/**
 * najde prvni volny zaznam
 */
Mft_Item *getFreeMftItem() {
	MFT_List *temp = head;
	while (temp != NULL) {
		if (temp->item->uid == FREE_ITEM) {
			debugs("Nasel jsem volny zaznam MFT\n");
			return temp->item;
		}
	}
	debugs("Tabulka MFT je plna\n");
	return NULL;
}
/**
 * Vymaze prvni zaznam
 */
void deleteFirst() {
	MFT_List *temp = head;

	pthread_rwlock_wrlock(&list_lock);
	head = head->next;
	pthread_rwlock_unlock(&list_lock);
	relaseMemory(temp);
}

/**
 * Uvolni pamet jenoho zaznamu z listu
 */
void relaseMemory(MFT_List *item){
	free(item->item);
	free(item);
}

/**
 * vytiskne jeden mft_item
 */
void printMftItem(Mft_Item *item) {
	printf("UID %d\n", item->uid);
	printf("isDirectory %d\n", item->isDirectory);
	printf("itemOrder %d\n", item->item_order);
	printf("itemName %s\n", item->item_name);
}
