#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "stringsearch_bmh.h"
#include "ss_strings.h"

/* case-insensitive strstr (ASCII) */
static int ref_istr(const char *text, const char *pat)
{
    size_t n = strlen(text), m = strlen(pat);
    for (size_t i = 0; i + m <= n; i++) {
        size_t k = 0;
        while (k < m && tolower((unsigned char)text[i + k]) == tolower((unsigned char)pat[k]))
            k++;
        if (k == m)
            return (int)i;
    }
    return -1;
}

int main(void)
{
    int pos_bmh[SS_N], pos_bmhi[SS_N], pos_bmha[SS_N];
    int errors = 0, found_cs = 0, found_ci = 0;

    stringsearch_bmh(input_patterns, input_texts, pos_bmh, pos_bmhi, pos_bmha);

    for (int i = 0; i < SS_N; i++) {
        const char *here = strstr(input_texts[i], input_patterns[i]);
        int exp_cs = here ? (int)(here - input_texts[i]) : -1;
        int exp_ci = ref_istr(input_texts[i], input_patterns[i]);
        if (pos_bmh[i] != exp_cs || pos_bmhi[i] != exp_ci || pos_bmha[i] != exp_ci) {
            printf("pair %d (\"%s\" in \"%s\"): got %d/%d/%d expected %d/%d/%d\n", i,
                   input_patterns[i], input_texts[i], pos_bmh[i], pos_bmhi[i], pos_bmha[i],
                   exp_cs, exp_ci, exp_ci);
            errors++;
        }
        found_cs += (exp_cs >= 0);
        found_ci += (exp_ci >= 0);
    }
    /* the case-insensitive searchers must find strictly more than the exact one */
    if (found_ci <= found_cs) {
        printf("expected case-insensitive hits (%d) > exact hits (%d)\n", found_ci, found_cs);
        errors++;
    }

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS (%d exact / %d case-insensitive hits of %d)\n", found_cs, found_ci, SS_N);
    return 0;
}
