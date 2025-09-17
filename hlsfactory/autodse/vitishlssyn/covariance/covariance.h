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

void kernel_covariance(int m,int n,double float_n,double data[100][80],double cov[80][80],double mean[80]);

#endif
