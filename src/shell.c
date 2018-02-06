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
						printf("-%s\n", temp->item_name);
					} else {
						printf("+%s\n", temp->item_name);
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

int cd(char *path) {
	Mft_Item *temp;
	if ((temp = parsePath(path)) == NULL) {
		return FALSE;
	}
	position = temp;
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
	if (isDirEmpty(dir) == FALSE) {
		return FALSE;
	}
	if (removeDir(dir, getMftItemByUID(dir->backUid, 1)) == TRUE) {
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
		nullMftItem(dir);
		writeChangeToFile();
		return TRUE;
	}

	return FALSE;
}

int mkdir(Mft_Item *parentDir, char *name) {
	Mft_Item *newDir;
	int bit;
	char stringUID[12];
	//overim jestli parentDir
	//zjistim jestli slozka uz slozku s timto nazvem neobsahuje
	if (dirContains(parentDir, name) != NULL) {
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
	newDir->item_size = 1;
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
	writeChangeToFile();

	return TRUE;
}

