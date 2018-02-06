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
int mkdir(Mft_Item *parentDir, char *name);
int pwd(void);
int cd(char *path);
int rmdir(Mft_Item *dir);

Mft_Item *parsePath(char *path);
Mft_Item *dirContains(Mft_Item *dir, char *nameOfFile);
int isDirEmpty(Mft_Item *dir);
int removeDir(Mft_Item *itemToRemove, Mft_Item *fromDir);
void nullMftItem(Mft_Item *item);
void writeChangeToFile();

#endif /* SHELL_H_ */
