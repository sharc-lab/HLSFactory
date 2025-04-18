/**
 * @author: Rafał Skinderowicz (rafal.skinderowicz@us.edu.pl)
 */
#include <omp.h>

#include "rand.h"

/**
 * This is used to get a default (pseudo-)random number generator (RNG).
 * Every OpenMP thread has its own private instance that is
 * initialized by calling init_random_number_generators().
 * All threads generators share the same sequence of the random numbers,
 * however they are looking at different (non-overlapping) parts of it.
 * This assumes that each thread does not generates more than 2^64 
 * numbers.
 *
 * @return Instance of the RNG for the calling thread.
 */
Random &get_rng() {
    static Random rng;
// #pragma omp threadprivate(rng)
    return rng;
}

/**
 * This should be called before drawing any random numbers.
 */
void init_random_number_generators(uint64_t seed) {
    // #pragma omp parallel default(none) shared(seed)
    {
        // Each thread sets its own generator to the same initial state
        get_rng().init(seed);

        // Now, each thread has to "jump" within the sequence -- we want
        // each thread to be in a different part of the whole RNG sequence.
        // OpenMP threads are numbered from 0 to n-1, so the number of jumps
        // is equal to the id of a thread.
        const int thread_id = omp_get_thread_num();
        for (int i = 0; i < thread_id; ++i) {
            get_rng().jump();
        }
    }
}
