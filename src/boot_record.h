/*
 * boot_record.h
 *
 *  Created on: 20.12.2017
 *      Author: jokertwo
 */
#include <stdint.h>


#ifndef BOOT_RECORD_H_GUARD
#define BOOT_RECORD_H_GUARD

typedef struct boot_record {
	char signature[9]; //login autora FS
	char volume_descriptor[251]; //popis vygenerovaného FS
	int32_t disk_size; //celkova velikost VFS
	int32_t cluster_size; //velikost clusteru
	int32_t cluster_count; //pocet clusteru
	int32_t mft_start_address; //adresa pocatku mft
	int32_t bitmap_start_address; //adresa pocatku bitmapy
	int32_t data_start_address; //adresa pocatku datovych bloku
	int32_t mft_max_fragment_count; //maximalni pocet fragmentu v jednom zaznamu v mft (pozor, ne souboru)
// stejne jako   MFT_FRAGMENTS_COUNT
}boot_record;

boot_record *boot;

#endif /* BOOT_RECORD_H_ */
