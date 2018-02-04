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


/**
 * vytvori bitmapu
 */
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
/**
 * uvolni pamet vyuzivanou pro bitmapu
 */
void disposeBitMap(void) {
	pthread_rwlock_destroy(&lock);
	free(bitmap);
}
/**
 * zapise bitmapu do souboru
 */
void writeToFile(int bitmapStartAdress,int clusterCount){
	fseek(fp,bitmapStartAdress,SEEK_SET);
	fwrite(bitmap, sizeof(int8_t), clusterCount / 8, fp);
}

/**
 * vytiskne bitove pole
 */
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

/**
 * Oznaci jeden bit jako obsazany
 */
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
/**
 * oznaci bit jako volny
 */
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
/**
 * vraci cislo volneho bitu
 */
int getFreeBit(int countOfClusters){
	int i;
	for(i = 1; i <= countOfClusters; i++){
		if(checkIfIsFree(i) == TRUE){
			return i;
		}
	}
	return FALSE;
}
/**
 * zkontroluje jestli je bit volny
 */
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
