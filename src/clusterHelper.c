/*
 * clusterHelper.c
 *
 *  Created on: 2.2.2018
 *      Author: jokertwo
 */

#include "clusterHelper.h"

int getAdressOfCluster(int numberOfCluster);

/**
 * vrati obsah daneho clusteru
 */
char *getClusterContent(int startAdress) {
	//alokoju si pamet pro obsah clusteru
	char *content = calloc(boot->cluster_size, sizeof(char));

	//najdu si adresu clusteru a
	//posunu se na dany cluster
	fseek(fp, startAdress, SEEK_SET);

	//prectu obsah clusteru
	fread(content, boot->cluster_size, 1, fp);

	return content;
}

int addToCluster(char *contentToAdd, int numberOfCluster) {
	//alokoju si pamet pro obsah clusteru
	char *content = calloc(boot->cluster_size, sizeof(char));

	fseek(fp, getAdressOfCluster(numberOfCluster), SEEK_SET);

	//prectu obsah clusteru
	fread(content, boot->cluster_size, 1, fp);

	if((strlen(content) + strlen(contentToAdd)) < boot->cluster_size){
		strcat(content,contentToAdd);
		fwrite(content,boot->cluster_size,1,fp);
		free(content);
		debugs("Do clusteru %d jsem pridal '%s'\n",numberOfCluster,contentToAdd);
		return TRUE;
	}
	free(content);
	debugs("Do clusteru %d se '%s' jiz nevejde.\n",numberOfCluster,contentToAdd);
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

