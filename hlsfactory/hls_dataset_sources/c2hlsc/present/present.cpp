#include "present.h"

void copyKey(present_key_t *from, present_key_t *to) {
    int i;
    for (i = 0; i < PRESENT_80_KEY_SIZE_BYTES; i++) {
        (*to)[i] = (*from)[i];
    }
}

void copyBlock(block_t *from, block_t *to) {
    int i;
    for (i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
        (*to)[i] = (*from)[i];
    }
}

void generateRoundKeys80(present_key_t *suppliedKey, keys_t *keys) {
    // tra,0x00,0x00shable key copies
    present_key_t key;
    present_key_t newKey;
    unsigned char i, j;
    copyKey(suppliedKey, &key);
    copyBlock((block_t *)&key, &((*keys)[0]));
    for (i = 1; i < ROUNDS; i++) {
        // rotate left 61 bits
        for (j = 0; j < PRESENT_80_KEY_SIZE_BYTES; j++) {
            newKey[j] = (key[(j + 7) % PRESENT_80_KEY_SIZE_BYTES] << 5) |
                        (key[(j + 8) % PRESENT_80_KEY_SIZE_BYTES] >> 3);
        }
        copyKey(&newKey, &key);

        // pass leftmost 4-bits through sBox
        key[0] = (sBox[key[0] >> 4] << 4) | (key[0] & 0xF);

        // xor roundCounter into bits 15 through 19
        key[8] ^= i << 7; // bit 15
        key[7] ^= i >> 1; // bits 19-16

        copyBlock((block_t *)&key, &((*keys)[i]));
    }
}

void addRoundKey(block_t *block, round_key_t *roundKey) {
    unsigned char i;
    for (i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
        (*block)[i] ^= (*roundKey)[i];
    }
}

void pLayer(block_t *block) {
    unsigned char i, j, indexVal, andVal;
    block_t initial;
    copyBlock(block, &initial);
    for (i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
        (*block)[i] = 0;
        for (j = 0; j < 8; j++) {
            indexVal = 4 * (i % 2) + (3 - (j >> 1));
            andVal = (8 >> (i >> 1)) << ((j % 2) << 2);
            (*block)[i] |= ((initial[indexVal] & andVal) != 0) << j;
        }
    }
}

void pLayerInverse(block_t *block) {
    unsigned char i, j, indexVal, andVal;
    block_t initial;
    copyBlock(block, &initial);
    for (i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
        (*block)[i] = 0;
        for (j = 0; j < 8; j++) {
            indexVal = (7 - ((2 * j) % 8)) - (i < 4);
            andVal = (7 - ((2 * i) % 8)) - (j < 4);
            (*block)[i] |= ((initial[indexVal] & (1 << andVal)) != 0) << j;
        }
    }
}

void present80_encryptBlock(block_t *block, present_key_t *key) {
    keys_t roundKeys;
    unsigned char i, j;
    generateRoundKeys80(key, &roundKeys);
    for (i = 0; i < ROUNDS - 1; i++) {
        addRoundKey(block, &(roundKeys[i]));
        for (j = 0; j < PRESENT_BLOCK_SIZE_BYTES; j++) {
            (*block)[j] =
                (sBox[(*block)[j] >> 4] << 4) | sBox[(*block)[j] & 0xF];
        }
        pLayer(block);
    }
    addRoundKey(block, &(roundKeys[ROUNDS - 1]));
}
