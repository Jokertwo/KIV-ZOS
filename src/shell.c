/*
 * shell.c
 *
 *  Created on: 2.2.2018
 *      Author: jokertwo
 */

#include "shell.h"
#include "pwdList.h"

int ls(Mft_Item *item) {
	int i = 1;
	do {
		//kontrola jestli je to slozka
		if (item->isDirectory == false) {
			debugs("ls: Operaci 'ls' lze volat jen na adresarem\n");
			return FALSE;
		}
		//projdu vsechny fragmenty (pravdepodobne slozka nikdy nebude na vice fragmentech ale co)
		for (int j = 0; j < MAX_FRAGMENT_COUNT; j++) {
			if (item->fragments[j].fragment_start_address == VOID) {
				continue;
			}
			//ziskam obsah bloku clusteru
			char *content = getClusterContent(
					item->fragments[j].fragment_start_address,
					item->fragments[j].fragment_count);
			//jednotliva UID jsou od sebe oddelena pomoci #
			char *token = strtok(content, DELIMETER);
			//pro kazde si vzhledam mft zaznam a zpracuji ho
			while (token != NULL) {
				Mft_Item *temp;
				int uid = atoi(token);
				if ((uid == FREE_ITEM) || ((temp = getMftItemByUID(uid, 1))
						== NULL)) {
					debugs("ls: Nelze na jit zaznam pro UDI=%s, ulozene v clusteru na adrese=%d\n",token,(
									item->fragments->fragment_start_address + (j
											* item->fragments->fragment_start_address)));
					return FALSE;
				} else {
					if (temp->isDirectory == true) {
						printf("+%s\n", temp->item_name);
					} else {
						printf("-%s\n", temp->item_name);
					}
				}
				token = strtok(NULL, DELIMETER);
			}
			//nakonec uvolnim blok clusteru a nactu si dalsi(v dalsi iteraci)
			free(content);
		}
		i++;
		//v pripade ze by slozka byla nejen ve vice fragmentech ale i ve vice mft itemech
		if (item->item_order_total > 1) {
			if ((item = getMftItemByUID(item->uid, i)) == NULL) {
				debugs("ls: Nepovedlo se najit zaznam v mft tabulce s item_order %d",i)
				return FALSE;
			}
		}
	} while (item->item_order_total >= i);
	return TRUE;
}
int cat(Mft_Item *item) {
	char *buffer;
	FILE *fp;
	if ((fp = fopen(fileName, "r")) == NULL) {
		debugs("reloadMftFromFile: napovedlo se otevrit soubor pro cteni");
	}
	for (int i = 1; i <= item->item_order_total; i++) {
		for (int j = 0; j < MAX_FRAGMENT_COUNT; j++) {
			if (item->fragments[j].fragment_start_address == VOID) {
				continue;
			}
			buffer = calloc(boot->cluster_size + 1,
					item->fragments[j].fragment_count);
			fseek(fp, item->fragments[j].fragment_start_address, SEEK_SET);
			fread(buffer, boot->cluster_size,
					item->fragments[j].fragment_count, fp);
			printf("%s\n", buffer);
			free(buffer);
		}
		if (i < item->item_order_total) {
			if ((item = getMftItemByUID(item->uid, i)) == NULL) {
				fclose(fp);
				return FALSE;
			}
		}

	}
	fclose(fp);
	return TRUE;
}

int cd(char *path) {
	Mft_Item *temp;
	if ((temp = parsePath(path, true)) == NULL) {
		return FALSE;
	}
	position = temp;
	return TRUE;
}
int cp(Mft_Item *from, Mft_Item *to, char *nameOfFile) {
	int *bits;
	int numberOfClusters;
	int index = 0;
	char *buffer;
	FILE *fp;
	Mft_Item *new;
	//zkontroluji jestli slozka uz soubor se strejnym jmenem neobsahuje
	if (dirContains(to, nameOfFile, false) != NULL) {
		printf("FILE ALREADY EXIST (Soubor %s ve slozce %s jiz existuje)\n",
				nameOfFile, to->item_name);
		return FALSE;
	}
	//zjistim kolik budu potrebovat clusteru a podle toho si reknu o bity
	numberOfClusters = getNumberOfClusters(from->item_size);
	if ((bits = getFreeBits(numberOfClusters, boot->cluster_count)) == NULL) {
		debugs("incp: Nedostatek volneho mista\n");
		return FALSE;
	}
	//podle rozlozeni bitu si zjistim kolik budu potrebovat fragmentu -->mft_itemu
	int countOfFragments = getNumberOfBitBlocks(bits, numberOfClusters);
	int countOfMftItems = (countOfFragments / MAX_FRAGMENT_COUNT)
			+ (countOfFragments % MAX_FRAGMENT_COUNT);

	//zjistim si jestli mam dost volnych mft
	if (getNumberOfFreeMft() < countOfMftItems) {
		free(bits);
		debugs("cp: Nedostatek mft zaznamu\n");
		printf("FAIL\n");
		return FALSE;
	}
	//otevru si soubor
	if ((fp = fopen(fileName, "r+b")) == NULL) {
		debugs("cp: Nepovedlo se otevrit soubor pro zapis.");
		free(bits);
		printf("FAIL\n");
		return FALSE;
	}
	//pokud ano vytvorim si mft zaznamy
	int newUID = getNewUID();
	int *tempBits = bits;
	buffer = bufferForCp(fp, from);
	char *tempBuffer = buffer;
	for (int i = 1; i <= countOfMftItems; i++) {
		new = getFreeMftItem();
		new->uid = newUID;
		strcpy(new->item_name, nameOfFile);
		new->backUid = to->uid;
		new->item_size = from->item_size;
		new->item_order = i;
		new->isDirectory = false;
		new->item_order_total = countOfMftItems;
		for (int j = 0; j < MAX_FRAGMENT_COUNT; j++) {
			new->fragments[j].fragment_start_address = boot->data_start_address
					+ ((*tempBits - 1) * boot->cluster_size);
			new->fragments[j].fragment_count = getSizeOfBitBlock(tempBits,
					numberOfClusters - index);
			fseek(fp, new->fragments[j].fragment_start_address, SEEK_SET);
			fwrite(tempBuffer, boot->cluster_size,
					new->fragments[j].fragment_count, fp);
			index += new->fragments[j].fragment_count;
			if (index < numberOfClusters) {
				tempBits = tempBits + new->fragments[j].fragment_count;
				tempBuffer = tempBuffer + (boot->cluster_size
						* new->fragments[j].fragment_count);
			} else {
				break;
			}
		}
	}
	free(buffer);
	fclose(fp);
	afterCp(numberOfClusters, bits, new, to);
	return TRUE;
}
int incp(char *nameOfFile, Mft_Item *item, FILE *file) {
	int fileSize;
	int numberOfClusters;
	int *bits;
	char *buffer;
	Mft_Item *new;
	FILE *fp;

	//zkontroluji jestli slozka uz soubor se strejnym jmenem neobsahuje
	if (dirContains(item, nameOfFile, false) != NULL) {
		printf("FILE ALREADY EXIST (Soubor %s ve slozce %s jiz existuje)\n",
				nameOfFile, item->item_name);
		return FALSE;
	}

	//zjistim si velikost souboru
	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);

	//zjistim kolik budu potrebovat clusteru a podle toho si reknu o bity
	numberOfClusters = getNumberOfClusters(fileSize);
	if ((bits = getFreeBits(numberOfClusters, boot->cluster_count)) == NULL) {
		debugs("incp: Nedostatek volneho mista\n");
		return FALSE;
	}

	//podle rozlozeni bitu si zjistim kolik budu potrebovat fragmentu -->mft_itemu
	int countOfFragments = getNumberOfBitBlocks(bits, numberOfClusters);

	int countOfMftItems = (countOfFragments / MAX_FRAGMENT_COUNT)
			+ (countOfFragments % MAX_FRAGMENT_COUNT);

	//zjistim si jestli mam dost volnych mft
	if (getNumberOfFreeMft() < countOfMftItems) {
		free(bits);
		debugs("cp: Nedostatek mft zaznamu\n");
		printf("FAIL\n");
		return FALSE;
	}
	int UID = getNewUID();
	int *tempBits = bits;
	int index = 0;

	//otevru soubor s ntfs
	if ((fp = fopen(fileName, "r+b")) == NULL) {
		debugs("incp: Napovedlo se otevrit soubor se souborovym systemem");
		free(bits);
		return FALSE;
	}
	rewind(file);
	//nactu si soubor z venku do bufferu
	buffer = calloc(fileSize + 1, sizeof(char));
	fread(buffer, fileSize, 1, file);
	char *tempBuffer = NULL;
	tempBuffer = buffer;

	//vytvorim mft_zaznamy a zapisu clustery do ntfs
	for (int i = 1; i <= countOfMftItems; i++) {
		new = getFreeMftItem();
		new->uid = UID;
		strncpy(new->item_name, nameOfFile, 11);
		new->backUid = item->uid;
		new->item_size = fileSize;
		new->item_order = i;
		new->isDirectory = false;
		new->item_order_total = countOfMftItems;
		for (int j = 0; j < MAX_FRAGMENT_COUNT; j++) {
			new->fragments[j].fragment_start_address = boot->data_start_address
					+ ((*tempBits - 1) * boot->cluster_size);
			new->fragments[j].fragment_count = getSizeOfBitBlock(tempBits,
					numberOfClusters - index);
			fseek(fp, new->fragments[j].fragment_start_address, SEEK_SET);
			fwrite(tempBuffer, boot->cluster_size,
					new->fragments[j].fragment_count, fp);
			index += new->fragments[j].fragment_count;
			if (index < numberOfClusters) {
				tempBits = tempBits + new->fragments[j].fragment_count;
				tempBuffer = tempBuffer + (boot->cluster_size
						* new->fragments[j].fragment_count);
			} else {
				break;
			}
		}
	}
	free(buffer);
	fclose(file);
	fclose(fp);
	afterCp(numberOfClusters, bits, new, item);

	return TRUE;

}

int pwd(void) {
	Mft_Item *tempPosition = position;
	//pokud jsem v rootu vytiskni jen lomitko
	if (tempPosition->backUid == ROOT_BACK_UID) {
		pushPwd("");
	}
	//pokud jsem v nejake slozce projdi stromem az k rootu
	else {
		while (tempPosition->backUid != ROOT_BACK_UID) {
			pushPwd(tempPosition->item_name);
			tempPosition = getMftItemByUID(tempPosition->backUid, 1);
		}
	}
	//vytiskne obsah listu, vymazeho a uvolni pamet
	if (!isPwdListEmpty()) {
		printPwdList();
		deletePwdList();
		return TRUE;
	}
	debugs("pwd: Pwd list je prazny a nema byt!\n");
	return FALSE;

}

int rmdir(Mft_Item *dir) {

	printBits(boot->cluster_count / 8, bitmap);
	for (int i = 1; i <= dir->item_order_total; i++) {
		if (removeFromDir(dir, getMftItemByUID(dir->backUid, i)) != TRUE) {
			debugs("rmdir: Nepovedlo se najit nadrazenou slozku\n");
			return FALSE;
		}
		//delete bitmap
		for (int i = 0; i < MAX_FRAGMENT_COUNT; i++) {
			for (int j = 0; j < dir->fragments[i].fragment_count; j++) {
				int bit = ((dir->fragments[i].fragment_start_address + (j
						* boot->cluster_size)) - boot->bitmap_start_address)
						/ boot->cluster_size;
				bit++;
				deleteBit(bit);

			}
		}
		if (i == dir->item_order_total) {
			updateSize(dir, false);
		}
		nullMftItem(dir);

	}
	printBits(boot->cluster_count / 8, bitmap);
	writeChangeToFile();
	return TRUE;
}

int mkdir(Mft_Item *parentDir, char *name) {
	Mft_Item *newDir;
	int bit;
	char stringUID[12];
	//overim jestli parentDir
	//zjistim jestli slozka uz slozku s timto nazvem neobsahuje
	if (dirContains(parentDir, name, true) != NULL) {
		printf("EXIST (nelze zalozit, jiz existuje\n");
		return FALSE;
	}
	//ziskam volny zaznam v mft
	if ((newDir = getFreeMftItem()) == NULL) {
		printf("NO FREE MFT_ITEM (zadny volny mft zaznam)\n");
		return FALSE;
	}
	//ziskam volny cluster/bit
	if ((bit = getFreeBit(boot->cluster_count)) == FALSE) {
		printf("NO LEFT SPACE ON DISK (na disku neni zadne volne misto)");
		return FALSE;
	}
	//vytvorim mft_item
	newDir->uid = getNewUID();
	newDir->isDirectory = true;
	strcpy(newDir->item_name, name);
	newDir->backUid = parentDir->uid;
	newDir->item_order = 1;
	newDir->item_order_total = 1;
	newDir->item_size = intLeng(newDir->uid) + 1;
	newDir->fragments[0].fragment_count = 1;
	int clusterAdress = boot->data_start_address + (boot->cluster_size * (bit
			- 1));
	newDir->fragments[0].fragment_start_address = clusterAdress;

	memset(stringUID, 0, 12);
	sprintf(stringUID, "%d%s", newDir->uid, DELIMETER);
	//zapisu do clusteru
	addToCluster(stringUID, parentDir->fragments[0].fragment_start_address);
	//zapisu do bitmapy
	writeBit(bit);
	//upravim velikost
	updateSize(newDir, true);
	//za[isu bitmapu a mft do souboru
	writeChangeToFile();

	return TRUE;
}

