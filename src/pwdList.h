/*
 * pwdList.h
 *
 *  Created on: 4.2.2018
 *      Author: jokertwo
 */

#ifndef PWDLIST_H_
#define PWDLIST_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "globalVariable.h"

typedef struct pwdList{
	char name[12];
	struct pwdList *next;
}pwdList;

void pushPwd(char *name);
void printPwdList(void);
bool isPwdListEmpty(void);
void deletePwdList(void);

#endif /* PWDLIST_H_ */
