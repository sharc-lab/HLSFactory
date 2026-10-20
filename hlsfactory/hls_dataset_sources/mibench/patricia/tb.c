#include <stdio.h>
#include <string.h>
#include "patricia.h"
#include "trace.h"

/* Reference: an address is "found" iff it appeared earlier in the trace. */
static int check(const char *name, const unsigned int *keys)
{
    static unsigned char found[PAT_N];
    int errors = 0, distinct = 0;

    int nodes = patricia_trace(keys, found);

    for (int i = 0; i < PAT_N; i++) {
        int seen = 0;
        for (int j = 0; j < i; j++)
            if (keys[j] == keys[i]) {
                seen = 1;
                break;
            }
        if (!seen)
            distinct++;
        if (found[i] != seen) {
            printf("%s: key %u at %d: found=%d expected %d\n", name, keys[i], i, found[i], seen);
            if (++errors > 10)
                break;
        }
    }
    if (nodes != distinct + 1) {
        printf("%s: %d nodes, expected %d\n", name, nodes, distinct + 1);
        errors++;
    }
    return errors;
}

int main(void)
{
    static unsigned int rnd[PAT_N];
    unsigned int s = 99;
    int errors = 0;

    /* full 32-bit address space with repeats and neighbouring addresses */
    for (int i = 0; i < PAT_N; i++) {
        s = s * 1664525u + 1013904223u;
        rnd[i] = (i % 5 == 4) ? rnd[i - 3] : ((i % 7 == 6) ? rnd[i - 1] ^ 1u : s);
        if (rnd[i] == 0)
            rnd[i] = 0x80000000u; /* 0 is the head's key */
    }

    errors += check("small.udp trace", input_trace);
    errors += check("random trace", rnd);

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS\n");
    return 0;
}
