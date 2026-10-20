#ifndef STRINGSEARCH_BMH_H
#define STRINGSEARCH_BMH_H

#define SS_N 57        /* number of (pattern, text) pairs */
#define SS_PAT_LEN 16  /* bytes per pattern, including the terminating NUL */
#define SS_TEXT_LEN 48 /* bytes per text, including the terminating NUL */

/*
 * Boyer-Moore-Horspool searches of the three MiBench variants for every
 * (pattern, text) pair.  Each result is the offset of the first match or -1:
 *   pos_bmh   case sensitive                       (bmhsrch.c)
 *   pos_bmhi  case insensitive                     (bmhisrch.c)
 *   pos_bmha  case insensitive, accented chars too (bmhasrch.c)
 */
void stringsearch_bmh(const char patterns[SS_N][SS_PAT_LEN],
                      const char texts[SS_N][SS_TEXT_LEN], int pos_bmh[SS_N],
                      int pos_bmhi[SS_N], int pos_bmha[SS_N]);

#endif
