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
int incp(char *nameOfFile, Mft_Item *item, FILE *file);
int cat(Mft_Item *item);
int cp(Mft_Item *from, Mft_Item *to, char *nameOfFile);
int mv(Mft_Item *from, Mft_Item *to, char *fileName);

Mft_Item *parsePath(char *path, bool isDir);
Mft_Item *dirContains(Mft_Item *dir, char *nameOfFile);
int isDirEmpty(Mft_Item *dir);
int removeFromDir(Mft_Item *itemToRemove, Mft_Item *fromDir);
void nullMftItem(Mft_Item *item);
void writeChangeToFile();
int updateSize(Mft_Item *startItem, bool upOrDown);
void printInfoAboutMftItem(Mft_Item *item);
int getNumberOfClusters(int fileSize);
int getSizeOfBitBlock(int *bits, int sizeOfArray);
int getNumberOfBitBlocks(int *bits, int sizeOfArray);
int intLeng(int n);
Resolut *destination(char *path, bool isDir);
char *bufferForCp(FILE *fp, Mft_Item *item);
void afterCp(int numberOfClusters, int *bits, Mft_Item *new,Mft_Item *parent);
int conCheck(int numberOfThread);


void *checkConsistency(void *arg);

#endif /* SHELL_H_ */
