/*
 * mft_fragments.h
 *
 *  Created on: 15.1.2018
 *      Author: jokertwo
 */

#ifndef MFT_FRAGMENTS_H_GUARD
#define MFT_FRAGMENTS_H_GUARD


typedef struct mft_fragment {
    int32_t fragment_start_address;     //start adresa
    int32_t fragment_count;             //pocet clusteru ve fragmentu
}mft_fragment;

#endif /* MFT_FRAGMENTS_H_ */
