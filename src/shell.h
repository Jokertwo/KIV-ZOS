/*
 * shell.h
 *
 *  Created on: 2.2.2018
 *      Author: jokertwo
 */

#ifndef SHELL_H_
#define SHELL_H_

#include "load.h"
#include "clusterHelper.h"
#include "bitMap.h"

int ls(Mft_Item *item);
int mkdir(int UID, char *name);
int pwd(void);
int cd(char *path);

Mft_Item *parsePath(char *path);

#endif /* SHELL_H_ */
