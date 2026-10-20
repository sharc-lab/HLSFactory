#ifndef HEADER_BLOWFISH_H
#define HEADER_BLOWFISH_H

#include <stdint.h>

#define BF_ROUNDS 16
#define BF_KEY_LEN 16   /* key bytes (MiBench uses a 128-bit key) */
#define BF_DATA_LEN 256 /* bytes processed per call (multiple of 8) */

/* Expanded key: P-array and four S-boxes */
typedef struct {
    uint32_t P[BF_ROUNDS + 2];
    uint32_t S[4 * 256];
} BF_KEY;

void BF_set_key(BF_KEY *key, int len, const uint8_t *data);
/* Encrypts one 64-bit block (two big-endian words) in place */
void BF_encrypt(uint32_t data[2], const BF_KEY *key);

/*
 * Expands the key and runs 64-bit cipher feedback mode (BF_cfb64_encrypt of
 * the original library, which is what MiBench's "bf e/d" does) over
 * BF_DATA_LEN bytes.  encrypt = 1 encrypts, 0 decrypts.
 */
void blowfish_cfb64(const uint8_t key[BF_KEY_LEN], const uint8_t ivec[8],
                    const uint8_t in[BF_DATA_LEN], uint8_t out[BF_DATA_LEN],
                    int encrypt);

#endif
