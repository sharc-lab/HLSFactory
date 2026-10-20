#ifndef PATRICIA_H
#define PATRICIA_H

#define PAT_N 2048                 /* addresses processed per call */
#define PAT_MAX_NODES (PAT_N + 1)  /* node pool: the head plus one node per address */

/*
 * Replays an address trace through a Patricia trie like patricia_test.c:
 * each address is looked up (pat_search); if it is already present
 * found[i] = 1, otherwise it is inserted (pat_insert) and found[i] = 0.
 * Returns the number of nodes in the trie at the end, head included.
 */
int patricia_trace(const unsigned int keys[PAT_N], unsigned char found[PAT_N]);

#endif
