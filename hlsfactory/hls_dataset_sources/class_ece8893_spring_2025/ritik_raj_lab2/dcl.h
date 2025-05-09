
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ap_fixed.h>
#include <hls_math.h>
#include <stdlib.h>
#include <cstdint>

typedef ap_fixed<16, 5> fixed_t;
// Using wide_t (1024 bits) datatype to fully utilize DRAM AXI bandwidth
typedef ap_uint<1024> wide_t;

// Define tensor dimensions
#define B   4      // Batch size
#define N   100     // Sequence length
#define dk  128     // Key/Query dimension
#define dv  128     // Value dimension

void compute_attention_HLS(wide_t Q[B][N][dk/64], wide_t K[B][N][dk/64], wide_t V[B][N][dv/64], wide_t Output[B][N][dv/64]);
