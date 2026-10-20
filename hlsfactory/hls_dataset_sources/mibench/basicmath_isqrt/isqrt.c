/* +++Date last modified: 05-Jul-1997 */
/*
 * MiBench automotive/basicmath (isqrt.c): bit-by-bit integer square root
 * from the SNIPPETS collection.
 *
 *  usqrt:
 *    ENTRY x: unsigned 32-bit
 *    EXIT  returns floor(sqrt(x) * pow(2, BITSPERLONG/2))
 *    The calculations are the base-two analogue of the square root
 *    algorithm learned in grammar school; no multiplications or divisions
 *    are performed.
 *
 * HLS adaptation: fixed 32-bit types, the result struct is returned as a
 * plain value, and the top function maps usqrt over an array.
 */
#include "isqrt.h"

#define BITSPERLONG 32
#define TOP2BITS(x) (((x) & (3u << (BITSPERLONG - 2))) >> (BITSPERLONG - 2))

static uint32_t usqrt(uint32_t x)
{
    uint64_t a = 0; /* accumulator (needs 33 bits for x close to 2^32) */
    uint64_t r = 0; /* remainder   */
    uint64_t e = 0; /* trial product */
    int i;

    for (i = 0; i < BITSPERLONG; i++) {
        r = (r << 2) + TOP2BITS(x);
        x <<= 2;
        a <<= 1;
        e = (a << 1) + 1;
        if (r >= e) {
            r -= e;
            a++;
        }
    }
    return (uint32_t)a;
}

void usqrt_array(const uint32_t in[ISQRT_N], uint32_t out[ISQRT_N])
{
    for (int i = 0; i < ISQRT_N; i++)
        out[i] = usqrt(in[i]);
}
