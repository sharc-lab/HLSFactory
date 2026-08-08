#pragma once

#include <stdio.h>

#define PRESENT_80_KEY_SIZE_BYTES 10
#define PRESENT_BLOCK_SIZE_BYTES 8
#define ROUNDS 32
#define ROUND_KEY_SIZE_BYTES 8
typedef unsigned char keys_t[ROUNDS][ROUND_KEY_SIZE_BYTES];
typedef unsigned char present_key_t[PRESENT_80_KEY_SIZE_BYTES];
typedef unsigned char round_key_t[ROUND_KEY_SIZE_BYTES];
typedef unsigned char block_t[PRESENT_BLOCK_SIZE_BYTES];

static const unsigned char sBox[16] = {0xC,
                                       0x5,
                                       0x6,
                                       0xB,
                                       0x9,
                                       0x0,
                                       0xA,
                                       0xD,
                                       0x3,
                                       0xE,
                                       0xF,
                                       0x8,
                                       0x4,
                                       0x7,
                                       0x1,
                                       0x2};

static const unsigned char sBoxInverse[16] = {0x5,
                                              0xE,
                                              0xF,
                                              0x8,
                                              0xC,
                                              0x1,
                                              0x2,
                                              0xD,
                                              0xB,
                                              0x4,
                                              0x6,
                                              0x3,
                                              0x0,
                                              0x7,
                                              0x9,
                                              0xA};

void present80_encryptBlock(block_t *block, present_key_t *key);