/*
 * shell.c
 *
 *  Created on: 2.2.2018
 *      Author: jokertwo
 */

#include "shell.h"

int ls(int UID) {
	Mft_Item *item;
	int i = 1;
	do {
		if ((item = getMftItemByUID(UID, i)) == NULL) {
			debugs("Nemohl jsem nalezt zaznam v tabulce s UID = %d\n",UID);
			return FALSE;
		}

		if (item->isDirectory == false) {
			debugs("Operaci 'ls' lze volat jen na adresarem\n");
			return FALSE;
		}

		for (int j = 0; j < item->fragments->fragment_count; j++) {
			char *content = getClusterContent(
					item->fragments->fragment_start_address + (j
							* item->fragments->fragment_start_address));
			char *token = strtok(content, "\n");
			while (token != NULL) {
				Mft_Item *temp;
				if ((temp = getMftItemByUID(atoi(token), 1)) == NULL) {
					debugs("Nelze na jit zaznam pro UDI=%s, ulozene v clusteru na adrese=%d\n",token,(
									item->fragments->fragment_start_address + (j
											* item->fragments->fragment_start_address)));
					return FALSE;
				}
				if (temp->isDirectory == true) {
					printf("-%s\n", temp->item_name);
				} else {
					printf("+%s\n", temp->item_name);
				}
				token = strtok(NULL, "\n");
			}
			free(content);
		}
		i++;
	} while (item->item_order_total >= i);
	return TRUE;
}
