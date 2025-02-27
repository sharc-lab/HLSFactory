#include "3mm.h"


void kernel_3mm(    
		double E[ 16 + 0][18 + 0],
		double A[ 16 + 0][20 + 0],
		double B[ 20 + 0][18 + 0],
		double F[ 18 + 0][22 + 0],
		double C[ 18 + 0][24 + 0],
		double D[ 24 + 0][22 + 0],
		double G[ 16 + 0][22 + 0])
{
  #pragma HLS top name=kernel_3mm

    const int ni = 16;
    const int nj = 18;
    const int nk = 20;
    const int nl = 22;
    const int nm = 24;

  int i, j, k;


  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++)
      {
	E[i][j] = 0.0;
	for (k = 0; k < nk; ++k)
	  E[i][j] += A[i][k] * B[k][j];
      }

  for (i = 0; i < nj; i++)
    for (j = 0; j < nl; j++)
      {
	F[i][j] = 0.0;
	for (k = 0; k < nm; ++k)
	  F[i][j] += C[i][k] * D[k][j];
      }

  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++)
      {
	G[i][j] = 0.0;
	for (k = 0; k < nj; ++k)
	  G[i][j] += E[i][k] * F[k][j];
      }

}