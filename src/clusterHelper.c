/*
 * clusterHelper.c
 *
 *  Created on: 2.2.2018
 *      Author: jokertwo
 */

#include "clusterHelper.h"

int getAdressOfCluster(int numberOfCluster);

/**
 * vrati obsah daneho bloku clusteru
 */
char *getClusterContent(int clusterAdress, int countOfClusters) {
	//alokoju si pamet pro obsah clusteru
	char *content = calloc(boot->cluster_size * countOfClusters, sizeof(char));

	//najdu si adresu clusteru a
	//posunu se na dany cluster
	fseek(fp, clusterAdress, SEEK_SET);

	//prectu obsah clusteru
	fread(content, boot->cluster_size, countOfClusters, fp);

	return content;
}

int addToCluster(char *contentToAdd, int clusterAdress) {
	//alokoju si pamet pro obsah clusteru
	char *content = calloc(boot->cluster_size, sizeof(char));

	fseek(fp, clusterAdress, SEEK_SET);

	//prectu obsah clusteru
	fread(content, boot->cluster_size, 1, fp);

	if ((strlen(content) + strlen(contentToAdd)) < boot->cluster_size) {
		strcat(content, contentToAdd);
		fseek(fp, clusterAdress, SEEK_SET);
		fwrite(content, boot->cluster_size, 1, fp);
		free(content);
		debugs("Do clusteru s adresou %d jsem pridal '%s'\n",clusterAdress,contentToAdd);
		return TRUE;
	}
	free(content);
	debugs("Do clusteru s adresou %d se '%s' jiz nevejde.\n",clusterAdress,contentToAdd);
	return FALSE;

}

/**
 * Vymaze dany cluster (prepise ho na nuly)
 */
void deleteCluster(int numberOfCluster) {

	char *content = calloc(boot->cluster_size, sizeof(char));
	fseek(fp, getAdressOfCluster(numberOfCluster), SEEK_SET);
	fwrite(content, boot->cluster_size, 1, fp);
	free(content);

}

/**
 * Vraci adresu daneho custeru
 */
int getAdressOfCluster(int numberOfCluster) {
	return boot->data_start_address + ((numberOfCluster - 1)
			* boot->cluster_size);
}


