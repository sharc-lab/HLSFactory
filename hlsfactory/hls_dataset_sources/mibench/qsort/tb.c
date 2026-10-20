#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qsort.h"
#include "words.h"

/* same ordering as the MiBench comparator */
static int cmp_desc(const void *a, const void *b)
{
    int r = strcmp((const char *)a, (const char *)b);
    return (r < 0) ? 1 : ((r == 0) ? 0 : -1);
}

int main(void)
{
    char dut[QSORT_N][QSORT_LEN];
    char ref[QSORT_N][QSORT_LEN];
    int errors = 0;

    memcpy(dut, input_words, sizeof(dut));
    memcpy(ref, input_words, sizeof(ref));

    qsort_words(dut);
    qsort(ref, QSORT_N, QSORT_LEN, cmp_desc);

    for (int i = 0; i < QSORT_N; i++) {
        if (strcmp(dut[i], ref[i]) != 0) {
            printf("index %d: got \"%s\" expected \"%s\"\n", i, dut[i], ref[i]);
            errors++;
        }
    }

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS\n");
    return 0;
}
