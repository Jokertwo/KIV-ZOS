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

int ls(int UID);
int mkdir(int UID, char *name);

#endif /* SHELL_H_ */
