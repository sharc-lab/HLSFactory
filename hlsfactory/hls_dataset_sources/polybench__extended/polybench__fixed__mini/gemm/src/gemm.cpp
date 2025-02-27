#include "gemm.h"


#include "ap_fixed.h"
#include "hls_math.h"


void kernel_gemm(  
		 t_ap_fixed alpha,
		 t_ap_fixed beta,
		 t_ap_fixed C[ 20 + 0][25 + 0],
		 t_ap_fixed A[ 20 + 0][30 + 0],
		 t_ap_fixed B[ 30 + 0][25 + 0])
{
  #pragma HLS top name=kernel_gemm

    const int ni = 20;
    const int nj = 25;
    const int nk = 30;

  int i, j, k;
  for (i = 0; i < ni; i++) {
    for (j = 0; j < nj; j++)
	C[i][j] *= beta;
    for (k = 0; k < nk; k++) {
       for (j = 0; j < nj; j++)
	  C[i][j] += alpha * A[i][k] * B[k][j];
    }
  }

}