/*
 * patricia.c
 *
 * Patricia trie implementation.
 *
 * Functions for inserting nodes and searching in a Patricia trie designed
 * for IP addresses and netmasks.  A head node must be created with
 * (key,mask) = (0,0).
 *
 * Matthew Smart <mcsmart@eecs.umich.edu>
 *
 * Copyright (c) 2000
 * The Regents of the University of Michigan
 * All rights reserved
 */
/*
 * MiBench network/patricia (patricia.c, patricia_test.c).
 *
 * HLS adaptation: the malloc'ed linked nodes are entries of a static node
 * pool addressed by index, the recursive insertR() is a loop, and the file
 * driver is replaced by a trace array.  Only pat_search() and pat_insert()
 * are used by the benchmark driver (pat_remove() is not called) and every
 * entry there has the host mask 0xffffffff, so a node carries exactly one
 * mask and the multi-mask bookkeeping of the original is not needed.
 */
#include "patricia.h"

typedef unsigned int u32;

typedef struct {
    u32 key[PAT_MAX_NODES];   /* node key */
    u32 mask[PAT_MAX_NODES];  /* node mask */
    int b[PAT_MAX_NODES];     /* bit to check */
    int left[PAT_MAX_NODES];  /* left child index */
    int right[PAT_MAX_NODES]; /* right child index */
    int count;                /* nodes in use */
} ptree_pool;

/* nonzero iff bit i (0 = most significant) is set in key */
static u32 bit(int i, u32 key)
{
    return (key >> (31 - i)) & 1u;
}

/* Find an entry given a key, longest-mask match; the head (index 0) matches everything */
static int pat_search(u32 key, const ptree_pool *t)
{
    int p = -1;
    int n = 0;
    int i;

    do {
        if (t->key[n] == (key & t->mask[n]))
            p = n;
        i = t->b[n];
        n = bit(t->b[n], key) ? t->right[n] : t->left[n];
    } while (i < t->b[n]);

    return (t->key[n] == (key & t->mask[n])) ? n : p;
}

/* Insert node n (already filled with key, mask) */
static int pat_insert(int n, ptree_pool *t)
{
    int tn, i;
    int owner, h, dir;

    /* Make sure the key matches the mask. */
    t->key[n] &= t->mask[n];

    /* Find closest matching leaf node. */
    tn = 0;
    do {
        i = t->b[tn];
        tn = bit(t->b[tn], t->key[n]) ? t->right[tn] : t->left[tn];
    } while (i < t->b[tn]);

    /* Same key and same mask: the existing entry is kept (no new node). */
    if (t->key[n] == t->key[tn] && t->mask[n] == t->mask[tn])
        return tn;

    /* Find the first bit that differs. */
    for (i = 1; i < 32 && bit(i, t->key[n]) == bit(i, t->key[tn]); i++)
        ;

    /* Recursive step of insertR(), as a loop over the link (owner, dir) */
    owner = 0;
    dir = bit(t->b[0], t->key[n]) ? 1 : 0;
    h = dir ? t->right[owner] : t->left[owner];
    while (!((t->b[h] >= i) || (t->b[h] <= t->b[owner]))) {
        owner = h;
        dir = bit(t->b[h], t->key[n]) ? 1 : 0;
        h = dir ? t->right[owner] : t->left[owner];
    }
    t->b[n] = i;
    t->left[n] = bit(i, t->key[n]) ? h : n;
    t->right[n] = bit(i, t->key[n]) ? n : h;
    if (dir)
        t->right[owner] = n;
    else
        t->left[owner] = n;

    return n;
}

int patricia_trace(const unsigned int keys[PAT_N], unsigned char found[PAT_N])
{
    static ptree_pool pool;

    /* head node: default route, key 0, mask 0, bit 0, points to itself */
    pool.key[0] = 0;
    pool.mask[0] = 0;
    pool.b[0] = 0;
    pool.left[0] = 0;
    pool.right[0] = 0;
    pool.count = 1;

    for (int k = 0; k < PAT_N; k++) {
        u32 addr = keys[k];
        int pfind = pat_search(addr, &pool);

        if (pool.key[pfind] == addr) {
            found[k] = 1;
        } else {
            int n = pool.count++;
            pool.key[n] = addr;
            pool.mask[n] = 0xffffffffu;
            pool.b[n] = 0;
            pool.left[n] = n;
            pool.right[n] = n;
            pat_insert(n, &pool);
            found[k] = 0;
        }
    }
    return pool.count;
}
