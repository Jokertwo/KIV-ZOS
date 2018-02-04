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
//v teto promene se bude udrzovat uid pro dalsi zaznam
int32_t freeUID = 0;
void updateUID(int32_t UID);

/**
 * vlozi zaznam do listu

 void push(Mft_Item *item) {
 MFT_List *new = (MFT_List *) malloc(sizeof(MFT_List));
 new->item = item;

 pthread_rwlock_wrlock(&list_lock);
 updateUID(new->item->uid);
 new->next = head;
 head = new;
 pthread_rwlock_unlock(&list_lock);
 }
 */
void push(Mft_Item *item) {
	MFT_List *temp;
	MFT_List *new = (MFT_List *) malloc(sizeof(MFT_List));
	new->item = item;
	new->next = NULL;
	updateUID(item->uid);
	if (head == NULL) {
		head = new;
	} else {
		temp = head;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = new;
	}

}

void writeMftToFile(void) {
	debugs("Zapisuju mft tabulku do souboru\n");
	int index = 0;
	MFT_List *temp = head;
	while (temp != NULL) {
		fseek(fp, boot->mft_start_address + (index * sizeof(Mft_Item)),
				SEEK_SET);
		fwrite(temp->item, sizeof(Mft_Item), 1, fp);
		index++;
		temp = temp->next;
	}

}

void updateUID(int32_t UID) {
	if (freeUID <= UID) {
		freeUID = UID + 2;
	}
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
 */bool isEmpty() {
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
int getNewUID(void) {
	int temp = freeUID;
	updateUID(freeUID);
	return temp;
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
		temp = temp->next;
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
void relaseMemory(MFT_List *item) {
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
