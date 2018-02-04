/*
 * globalVariable.h
 *
 *  Created on: 20.12.2017
 *      Author: jokertwo
 */

#ifndef GLOBALVARIABLE_H_GUARD
#define GLOBALVARIABLE_H_GUARD
#include <stdio.h>
#define TRUE 1
#define FALSE -1
#define DELIMETER "#"

#define FREE_ITEM 0
int debug;
#define debugs(args ...) if(debug) fprintf(stderr,args);
#define error_log(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); }

//soubor s file-systemem
FILE *fp;

#endif /* GLOBALVARIABLE_H_ */

