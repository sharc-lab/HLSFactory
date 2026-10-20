#include <stdio.h>
#include <stdint.h>
#include "bitcount.h"

int main(void)
{
    const unsigned int seeds[] = {1804289383u, 0u, 0xFFFFFF00u};
    int errors = 0;

    for (unsigned int k = 0; k < sizeof(seeds) / sizeof(seeds[0]); k++) {
        int totals[BITCOUNT_FUNCS];
        int expected = 0;
        uint32_t s = seeds[k];

        for (int j = 0; j < BITCOUNT_ITERATIONS; j++, s += 13)
            expected += __builtin_popcount(s);

        bitcnts(seeds[k], totals);

        for (int f = 0; f < BITCOUNT_FUNCS; f++) {
            if (totals[f] != expected) {
                printf("seed %u algorithm %d: got %d expected %d\n",
                       seeds[k], f, totals[f], expected);
                errors++;
            }
        }
    }

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS\n");
    return 0;
}
