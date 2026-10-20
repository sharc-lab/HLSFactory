#include <stdio.h>
#include <stdint.h>
#include "isqrt.h"

/* floor(sqrt(x) * 65536) == floor(sqrt(x * 2^32)) computed exactly */
static uint32_t ref_usqrt(uint32_t x)
{
    uint64_t v = (uint64_t)x << 32;
    uint64_t lo = 0, hi = 0xFFFFFFFFull; /* sqrt(x * 2^32) < 2^32 */
    while (lo < hi) {
        uint64_t mid = lo + (hi - lo + 1) / 2;
        if (mid * mid <= v)
            lo = mid;
        else
            hi = mid - 1;
    }
    return (uint32_t)lo;
}

int main(void)
{
    static uint32_t in[ISQRT_N], out[ISQRT_N];
    int errors = 0;
    uint32_t s = 1;

    /* basicmath_small: 0..1000 and 0x3fed0169, then random values */
    for (int i = 0; i < 1001; i++)
        in[i] = (uint32_t)i;
    in[1001] = 0x3fed0169u;
    in[1002] = 0xFFFFFFFFu;
    for (int i = 1003; i < ISQRT_N; i++) {
        s = s * 1103515245u + 12345u;
        in[i] = s;
    }

    usqrt_array(in, out);

    for (int i = 0; i < ISQRT_N; i++) {
        uint32_t exp = ref_usqrt(in[i]);
        if (out[i] != exp) {
            printf("sqrt(%u): got %u expected %u\n", in[i], out[i], exp);
            errors++;
        }
    }
    /* sqrt(144) = 12 * 65536 (documented example) */
    if (out[144] != 12u * 65536u) {
        printf("sqrt(144) mismatch\n");
        errors++;
    }

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS\n");
    return 0;
}
