#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define Nb 4
#define Nk 4  // The number of 32 bit words in a key.
#define Nr 10 // The number of rounds in AES Cipher.

#define AES_KEYLEN 16 // Key length in bytes
#define AES_keyExpSize 176

typedef uint8_t state_t[4][4];
struct AES_ctx {
    uint8_t RoundKey[AES_keyExpSize];
};

void MixColumns(state_t *state);