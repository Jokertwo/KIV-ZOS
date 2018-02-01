/*
 * BitMap.c
 *
 *  Created on: 14.1.2018
 *      Author: jokertwo
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "bitMap.h"



int createBitMap(int numberOfClusters) {
	if ((bitmap = (int8_t*) calloc(numberOfClusters, sizeof(int8_t))) == NULL) {
		error_log("Can't allocate memory for bit map");
		return FALSE;
	}
	if (pthread_rwlock_init(&lock, NULL) != 0) {
		error_log("Cant inicialize rw_lock server cant start.\n");
		return FALSE;
	}
	return TRUE;
}
void disposeBitMap() {
	pthread_rwlock_destroy(&lock);
	free(bitmap);
}

void printBits(int sizeOfBitmap,int8_t *bitmap2) {
	for (int i = 0; i < sizeOfBitmap; i++) {
		unsigned char *b = (unsigned char*) &bitmap2[i];
		unsigned char byte;
		int i, j;

		for (i = sizeof(int8_t) - 1; i >= 0; i--) {
			for (j = 7; j >= 0; j--) {
				byte = (b[i] >> j) & 1;
				printf("%u", byte);
			}
			printf(" ");
		}
		puts("");
	}
	puts("----");
}

int writeBit(int bit) {
	if (bit < 0) {
		error_log("Bit can't be negative number");
		return FALSE;
	}
	int8_t mask = 1;
	mask = mask << iOfBit(bit);
	pthread_rwlock_wrlock(&lock);
	bitmap[iToBitMap(bit)] = mask | bitmap[iToBitMap(bit)];
	pthread_rwlock_unlock(&lock);
	return TRUE;
}
int deleteBit(int bit) {
	if (bit < 0) {
		error_log("Bit can't be negative number");
		return FALSE;
	}
	int8_t mask = 1;
	mask = mask << iOfBit(bit);
	mask = ~mask;
	pthread_rwlock_wrlock(&lock);
	bitmap[iToBitMap(bit)] = mask & bitmap[iToBitMap(bit)];
	pthread_rwlock_unlock(&lock);
	return TRUE;
}
int checkIfIsFree(int bit) {
	if (bit < 0) {
		error_log("Bit can't be negative number");
		return FALSE;
	}

	int8_t mask = 1;
	mask = mask << iOfBit(bit);
	pthread_rwlock_rdlock(&lock);
	int8_t res = mask & bitmap[iToBitMap(bit)];
	pthread_rwlock_unlock(&lock);
	if (res == 0) {
		return TRUE;
	}
	return FALSE;

}
