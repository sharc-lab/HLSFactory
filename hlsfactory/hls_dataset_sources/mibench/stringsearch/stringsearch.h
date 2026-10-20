#ifndef STRINGSEARCH_H
#define STRINGSEARCH_H

#define SS_N 57        /* number of (pattern, text) pairs */
#define SS_PAT_LEN 16  /* bytes per pattern, including the terminating NUL */
#define SS_TEXT_LEN 48 /* bytes per text, including the terminating NUL */

/*
 * For each pair, pos[i] is the offset of the first occurrence of
 * patterns[i] in texts[i], or -1 if the pattern does not occur.
 */
void stringsearch(const char patterns[SS_N][SS_PAT_LEN],
                  const char texts[SS_N][SS_TEXT_LEN], int pos[SS_N]);

#endif
