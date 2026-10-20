#include <stdio.h>
#include <string.h>
#include "stringsearch.h"
#include "ss_strings.h"

int main(void)
{
    int pos[SS_N];
    int errors = 0, found = 0;

    stringsearch(input_patterns, input_texts, pos);

    for (int i = 0; i < SS_N; i++) {
        const char *here = strstr(input_texts[i], input_patterns[i]);
        int exp = here ? (int)(here - input_texts[i]) : -1;
        if (pos[i] != exp) {
            printf("pair %d (\"%s\" in \"%s\"): got %d expected %d\n", i,
                   input_patterns[i], input_texts[i], pos[i], exp);
            errors++;
        }
        found += (exp >= 0);
    }
    /* the benchmark input contains both hits and misses */
    if (found == 0 || found == SS_N) {
        printf("unexpected hit count %d\n", found);
        errors++;
    }

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS (%d/%d patterns found)\n", found, SS_N);
    return 0;
}
