#ifndef ISQRT_H
#define ISQRT_H

#include <stdint.h>

#define ISQRT_N 1024

/*
 * out[i] = floor(sqrt(in[i]) * 65536), i.e. the integer square root with
 * 16 fractional bits (the 'sqrt' and 'frac' fields of struct int_sqrt are
 * out[i] >> 16 and out[i] & 0xffff).
 */
void usqrt_array(const uint32_t in[ISQRT_N], uint32_t out[ISQRT_N]);

#endif
