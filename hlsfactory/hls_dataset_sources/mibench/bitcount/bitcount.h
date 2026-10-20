#ifndef BITCOUNT_H
#define BITCOUNT_H

#define BITCOUNT_FUNCS 7
#define BITCOUNT_ITERATIONS 1000

/*
 * Runs the seven MiBench bit-counting algorithms (bit_count, bitcount,
 * ntbl_bitcnt, ntbl_bitcount, BW_btbl_bitcount, AR_btbl_bitcount,
 * bit_shifter) on BITCOUNT_ITERATIONS words (seed, seed+13, seed+26, ...)
 * and returns the total number of set bits found by each algorithm.
 */
void bitcnts(unsigned int seed, int totals[BITCOUNT_FUNCS]);

#endif
