#include "2mm.h"


#include "ap_fixed.h"
#include "hls_math.h"


void kernel_2mm(   
		t_ap_fixed alpha,
		t_ap_fixed beta,
		t_ap_fixed tmp[ 16 + 0][18 + 0],
		t_ap_fixed A[ 16 + 0][22 + 0],
		t_ap_fixed B[ 22 + 0][18 + 0],
		t_ap_fixed C[ 18 + 0][24 + 0],
		t_ap_fixed D[ 16 + 0][24 + 0])
{
  #pragma HLS top name=kernel_2mm

    const int ni = 16;
    const int nj = 18;
    const int nk = 22;
    const int nl = 24;

  int i, j, k;


  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++)
      {
	tmp[i][j] = (t_ap_fixed(0.0));
	for (k = 0; k < nk; ++k)
	  tmp[i][j] += alpha * A[i][k] * B[k][j];
      }
  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++)
      {
	D[i][j] *= beta;
	for (k = 0; k < nj; ++k)
	  D[i][j] += tmp[i][k] * C[k][j];
      }

}