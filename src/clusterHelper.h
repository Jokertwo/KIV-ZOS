/*
 * clusterHelper.h
 *
 *  Created on: 2.2.2018
 *      Author: jokertwo
 */

#ifndef CLUSTERHELPER_H_
#define CLUSTERHELPER_H_

#include <stdlib.h>
#include <string.h>

#include "load.h"

char *getClusterContent(int clusterAdress);
void deleteCluster(int clusterAdress);
int addToCluster(char *contentToAdd, int clusterAdress);

#endif /* CLUSTERHELPER_H_ */
