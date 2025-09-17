#ifndef TOP_H
#define TOP_H

#include "ap_fixed.h"
#include "hls_fft.h"
#include <complex>
#include <hls_stream.h>
#include <iomanip>   // std::setprecision, std::setw
#include <iostream>  // std::cout, std::fixed
#include <cmath>
#include "hls_vector.h"
#include "hls_streamofblocks.h"

void kernel_syr2k(double alpha,double beta,double C[80][80],double A[80][60],double B[80][60]);

#endif
