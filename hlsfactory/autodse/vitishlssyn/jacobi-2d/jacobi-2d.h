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

void kernel_jacobi_2d(int tsteps,int n,double A[90][90],double B[90][90]);

#endif
