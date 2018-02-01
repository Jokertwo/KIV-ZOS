/*
 * mftHelper.c
 *
 *  Created on: 1.2.2018
 *      Author: jokertwo
 */

#include "mftHelper.h"
#include <stdlib.h>
#include <stdio.h>

void printMftItem(Mft_Item *item);
void deleteFirst(void);
MFT_List *head = NULL;

void push(Mft_Item *item){
	MFT_List *new = (MFT_List * ) malloc(sizeof(MFT_List));
	new->item = item;

	pthread_rwlock_wrlock(&list_lock);
	new->next = head;
	head = new;
	pthread_rwlock_unlock(&list_lock);
}

void printList(){
	MFT_List *temp = head;

	pthread_rwlock_rdlock(&list_lock);
	while(temp != NULL){
		printMftItem(temp->item);
		temp = temp->next;
	}
	pthread_rwlock_unlock(&list_lock);

}
void clearList(){
	while(!isEmpty()){
		deleteFirst();
	}
}
bool isEmpty(){
   if(head != NULL){
	   return false;
   }
   return true;
}
void deleteFirst(){
	MFT_List *temp = head;

	pthread_rwlock_wrlock(&list_lock);
	head = head->next;
	pthread_rwlock_unlock(&list_lock);

	free(temp->item);
	free(temp);
}

void printMftItem(Mft_Item *item) {
	printf("UID %d\n", item->uid);
	printf("isDirectory %d\n", item->isDirectory);
	printf("itemOrder %d\n", item->item_order);
	printf("itemName %s\n", item->item_name);
}
