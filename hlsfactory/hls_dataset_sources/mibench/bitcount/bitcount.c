/*
 * MiBench automotive/bitcount: bit counting algorithms from the SNIPPETS
 * collection (bitcnt_1.c .. bitcnt_4.c, bitcnts.c), public domain by
 * Ratko Tomic, Auke Reitsma, Bruce Wedding, Bob Stout.
 *
 * HLS adaptation: 32-bit unsigned words instead of 'long', the function
 * pointer table is replaced by direct calls, the recursive nybble counter
 * is turned into a loop, and timing/printing is removed.
 */
#include <stdint.h>
#include "bitcount.h"

static const uint8_t bits[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

/* Optimized 1 bit/loop counter (bitcnt_1.c, Ratko Tomic) */
static int bit_count(uint32_t x)
{
    int n = 0;
    if (x) {
        do {
            n++;
        } while (0 != (x = x & (x - 1)));
    }
    return n;
}

/* Parallel "mystery algorithm" (bitcnt_2.c, Ratko Tomic) */
static int bitcount(uint32_t i)
{
    i = ((i & 0xAAAAAAAAu) >> 1) + (i & 0x55555555u);
    i = ((i & 0xCCCCCCCCu) >> 2) + (i & 0x33333333u);
    i = ((i & 0xF0F0F0F0u) >> 4) + (i & 0x0F0F0F0Fu);
    i = ((i & 0xFF00FF00u) >> 8) + (i & 0x00FF00FFu);
    i = ((i & 0xFFFF0000u) >> 16) + (i & 0x0000FFFFu);
    return (int)i;
}

/* Count bits in each nybble, originally recursive (bitcnt_4.c) */
static int ntbl_bitcnt(uint32_t x)
{
    int cnt = 0;
    for (int i = 0; i < 8; i++) {
        cnt += bits[x & 0x0000000Fu];
        x >>= 4;
    }
    return cnt;
}

/* Count bits in each nybble, non-recursive (bitcnt_3.c) */
static int ntbl_bitcount(uint32_t x)
{
    return bits[x & 0x0000000Fu] +
           bits[(x & 0x000000F0u) >> 4] +
           bits[(x & 0x00000F00u) >> 8] +
           bits[(x & 0x0000F000u) >> 12] +
           bits[(x & 0x000F0000u) >> 16] +
           bits[(x & 0x00F00000u) >> 20] +
           bits[(x & 0x0F000000u) >> 24] +
           bits[(x & 0xF0000000u) >> 28];
}

/* Count bits in each byte, Bruce Wedding (bitcnt_3.c); union replaced by shifts */
static int BW_btbl_bitcount(uint32_t x)
{
    uint8_t ch0 = x & 0xFF;
    uint8_t ch1 = (x >> 8) & 0xFF;
    uint8_t ch2 = (x >> 16) & 0xFF;
    uint8_t ch3 = (x >> 24) & 0xFF;
    return bits[ch0] + bits[ch1] + bits[ch3] + bits[ch2];
}

/* Count bits in each byte, Auke Reitsma (bitcnt_3.c); pointer walk replaced by shifts */
static int AR_btbl_bitcount(uint32_t x)
{
    int accu;
    accu = bits[x & 0xFF];
    accu += bits[(x >> 8) & 0xFF];
    accu += bits[(x >> 16) & 0xFF];
    accu += bits[(x >> 24) & 0xFF];
    return accu;
}

/* Shift and count bits (bitcnts.c) */
static int bit_shifter(uint32_t x)
{
    int i, n;
    for (i = n = 0; x && (i < 32); ++i, x >>= 1)
        n += (int)(x & 1u);
    return n;
}

void bitcnts(unsigned int seed, int totals[BITCOUNT_FUNCS])
{
    int n0 = 0, n1 = 0, n2 = 0, n3 = 0, n4 = 0, n5 = 0, n6 = 0;
    uint32_t s = seed;

    for (int j = 0; j < BITCOUNT_ITERATIONS; j++, s += 13) {
        n0 += bit_count(s);
        n1 += bitcount(s);
        n2 += ntbl_bitcnt(s);
        n3 += ntbl_bitcount(s);
        n4 += BW_btbl_bitcount(s);
        n5 += AR_btbl_bitcount(s);
        n6 += bit_shifter(s);
    }

    totals[0] = n0;
    totals[1] = n1;
    totals[2] = n2;
    totals[3] = n3;
    totals[4] = n4;
    totals[5] = n5;
    totals[6] = n6;
}
