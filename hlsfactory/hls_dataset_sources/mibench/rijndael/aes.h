#ifndef AES_H
#define AES_H

#include <stdint.h>

#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 16   /* 128-bit key */
#define AES_NR 10         /* rounds */
#define AES_NUM_BLOCKS 16
#define AES_DATA_LEN (AES_BLOCK_SIZE * AES_NUM_BLOCKS)

/*
 * Rijndael with 128-bit block and 128-bit key in ECB mode over
 * AES_DATA_LEN bytes.  decrypt = 0 encrypts, decrypt = 1 decrypts.
 */
void rijndael_ecb(const uint8_t key[AES_KEY_SIZE], const uint8_t in[AES_DATA_LEN],
                  uint8_t out[AES_DATA_LEN], int decrypt);

#endif
