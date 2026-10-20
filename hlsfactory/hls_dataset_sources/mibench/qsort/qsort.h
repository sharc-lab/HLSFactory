#ifndef QSORT_H
#define QSORT_H

#define QSORT_N 64   /* number of words */
#define QSORT_LEN 16 /* bytes per word, including the terminating NUL */

/*
 * Sorts the words in place with the comparison of qsort_small.c, which
 * returns 1 for strcmp(a, b) < 0: the result is in descending order.
 */
void qsort_words(char words[QSORT_N][QSORT_LEN]);

#endif
