#include "syrk.h"


#include "ap_fixed.h"
#include "hls_math.h"


void kernel_syrk( 
		 t_ap_fixed alpha,
		 t_ap_fixed beta,
		 t_ap_fixed C[ 30 + 0][30 + 0],
		 t_ap_fixed A[ 30 + 0][20 + 0])
{
  #pragma HLS top name=kernel_syrk

    const int n = 30;
    const int m = 20;

  int i, j, k;
  for (i = 0; i < n; i++) {
    for (j = 0; j <= i; j++)
      C[i][j] *= beta;
    for (k = 0; k < m; k++) {
      for (j = 0; j <= i; j++)
        C[i][j] += alpha * A[i][k] * A[j][k];
    }
  }

}