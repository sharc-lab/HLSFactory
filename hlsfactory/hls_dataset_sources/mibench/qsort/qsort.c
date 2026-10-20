/*
 * MiBench automotive/qsort (qsort_small.c): sorts a list of words.
 *
 * HLS adaptation: the C library qsort() (function-pointer comparator, heap
 * and file input) is replaced by an iterative quicksort with an explicit
 * stack over a fixed-size word table.  The ordering is the one produced by
 * the original comparator: descending strcmp order.
 */
#include "qsort.h"

#define STACK_DEPTH 32

/* Original comparator: result < 0 ? 1 : (result == 0 ? 0 : -1) */
static int compare(const char a[QSORT_LEN], const char b[QSORT_LEN])
{
    int result = 0;
    for (int i = 0; i < QSORT_LEN; i++) {
        if (a[i] != b[i]) {
            result = (unsigned char)a[i] - (unsigned char)b[i];
            break;
        }
        if (a[i] == '\0')
            break;
    }
    return (result < 0) ? 1 : ((result == 0) ? 0 : -1);
}

static void swap(char a[QSORT_LEN], char b[QSORT_LEN])
{
    for (int i = 0; i < QSORT_LEN; i++) {
        char t = a[i];
        a[i] = b[i];
        b[i] = t;
    }
}

void qsort_words(char words[QSORT_N][QSORT_LEN])
{
    int lo_stack[STACK_DEPTH];
    int hi_stack[STACK_DEPTH];
    int sp = 0;

    lo_stack[0] = 0;
    hi_stack[0] = QSORT_N - 1;
    sp = 1;

    while (sp > 0) {
        sp--;
        int lo = lo_stack[sp];
        int hi = hi_stack[sp];

        if (lo < hi) {
            /* Lomuto partition around the last element */
            int store = lo;
            for (int i = lo; i < hi; i++) {
                if (compare(words[i], words[hi]) < 0) {
                    swap(words[i], words[store]);
                    store++;
                }
            }
            swap(words[store], words[hi]);

            /* push the larger side first so that the stack stays O(log N) */
            if (store - lo > hi - store) {
                lo_stack[sp] = lo;
                hi_stack[sp] = store - 1;
                sp++;
                lo_stack[sp] = store + 1;
                hi_stack[sp] = hi;
                sp++;
            } else {
                lo_stack[sp] = store + 1;
                hi_stack[sp] = hi;
                sp++;
                lo_stack[sp] = lo;
                hi_stack[sp] = store - 1;
                sp++;
            }
        }
    }
}
