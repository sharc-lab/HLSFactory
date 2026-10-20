/* +++Date last modified: 05-Jul-1997 */
/*
 * MiBench office/stringsearch (pbmsrch_small.c): a Pratt-Boyer-Moore
 * string search, written by Jerry Coffin in 1991, corrected with help from
 * Thad Smith, revised by Bob Stout.  Placed in the Public Domain by its
 * author.  10/21/93 rdg  Fixed bug found by Jeff Dunlop.
 *
 * HLS adaptation: the file-scope table/pattern state of init_search() and
 * strsearch() is local to each search, strlen/strncmp are explicit loops
 * over the fixed-size buffers, and the result is an offset instead of a
 * pointer.
 */
#include "stringsearch.h"

#define ALPHABET 256

static int str_length(const char *s, int max)
{
    int n = 0;
    for (int i = 0; i < max; i++) {
        if (s[i] == '\0')
            break;
        n++;
    }
    return n;
}

/* Returns the offset of the first match of pattern in string, or -1 */
static int search_one(const char pattern[SS_PAT_LEN], const char string[SS_TEXT_LEN])
{
    int table[ALPHABET];
    int len = str_length(pattern, SS_PAT_LEN);
    int limit = str_length(string, SS_TEXT_LEN);
    int pos = len - 1;
    int shift = 0;

    /* init_search() */
    for (int i = 0; i < ALPHABET; i++)
        table[i] = len;
    for (int i = 0; i < SS_PAT_LEN; i++) {
        if (i < len)
            table[(unsigned char)pattern[i]] = len - i - 1;
    }

    /* strsearch() */
    while (pos < limit) {
        while (pos < limit &&
               (shift = table[(unsigned char)string[pos]]) > 0) {
            pos += shift;
        }
        if (0 == shift) {
            int here = pos - len + 1;
            int equal = 1;
            for (int k = 0; k < SS_PAT_LEN; k++) {
                if (k < len && string[here + k] != pattern[k])
                    equal = 0;
            }
            if (equal)
                return here;
            else
                pos++;
        }
    }
    return -1;
}

void stringsearch(const char patterns[SS_N][SS_PAT_LEN],
                  const char texts[SS_N][SS_TEXT_LEN], int pos[SS_N])
{
    for (int i = 0; i < SS_N; i++)
        pos[i] = search_one(patterns[i], texts[i]);
}
