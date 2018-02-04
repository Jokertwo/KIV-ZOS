/*
 * pwdList.c
 *
 *  Created on: 4.2.2018
 *      Author: jokertwo
 */

#include "pwdList.h"

pwdList *headPwd = NULL;
void deleteFirstPwd();

void pushPwd(char *name) {
	pwdList *new = (pwdList *) malloc(sizeof(pwdList));
	strcpy(new->name, name);
	new->next = headPwd;
	headPwd = new;
}
void printPwdList(void) {
	pwdList *temp = headPwd;
	while (temp != NULL) {
		printf("/%s", temp->name);
		temp = temp->next;
	}
	printf("\n");
}
bool isPwdListEmpty() {
	if (headPwd == NULL) {
		return true;
	}
	return false;
}
void deletePwdList() {
	while (!isPwdListEmpty()) {
		deleteFirstPwd();
	}
}

/**
 * Vymaze prvni zaznam
 */
void deleteFirstPwd() {
	pwdList *temp = headPwd;
	headPwd = headPwd->next;
	free(temp);
}
