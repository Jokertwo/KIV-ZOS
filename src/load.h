/*
 * load.h
 *
 *  Created on: 29.1.2018
 *      Author: jokertwo
 */

#ifndef LOAD_H_GUARD
#define LOAD_H_GUARD

#include "mftList.h"
#include "boot_record.h"
#include "globalVariable.h"

void *readBoot(void *arg);
void clean();

#endif /* LOAD_H_ */
