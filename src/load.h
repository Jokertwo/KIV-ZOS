/*
 * load.h
 *
 *  Created on: 29.1.2018
 *      Author: jokertwo
 */

#ifndef LOAD_H_
#define LOAD_H_

#include "mft_item.h"
#include "boot_record.h"
#include "globalVariable.h"

void *readBoot(void *arg);
void printMftItem(mft_item *item);
#endif /* LOAD_H_ */
