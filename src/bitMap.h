/*
 * bitMap.h
 *
 *  Created on: 14.1.2018
 *      Author: jokertwo
 */

#ifndef BITMAP_H_
#define BITMAP_H_

#include <pthread.h>
#include "globalVariable.h"

#define SIZE 8

#define error_log(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); }
#define iToBitMap(bit) (bit - 1) / SIZE
#define iOfBit(bit) (bit - 1) % SIZE

int8_t *bitmap;

pthread_rwlock_t lock;

#endif /* BITMAP_H_ */

void printBits(int sizeOfBitmap, int8_t *bitmap2);
void disposeBitMap(void);
int writeBit(int bit);
int deleteBit(int bit);
int checkIfIsFree(int bit);
int createBitMap(int numberOfClusters);
void writeToFile(int bitmapStartAdress,int clusterCount);
int getFreeBit(int countOfClusters);
