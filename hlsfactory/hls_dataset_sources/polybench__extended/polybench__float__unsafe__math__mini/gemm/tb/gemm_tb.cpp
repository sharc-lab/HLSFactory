#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "gemm.h"


void init_array(int ni, int nj, int nk,
		double *alpha,
		double *beta,
		double C[ 20 + 0][25 + 0],
		double A[ 20 + 0][30 + 0],
		double B[ 30 + 0][25 + 0])
{
  int i, j;

  *alpha = 1.5;
  *beta = 1.2;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++)
      C[i][j] = (double) ((i*j+1) % ni) / ni;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nk; j++)
      A[i][j] = (double) (i*(j+1) % nk) / nk;
  for (i = 0; i < nk; i++)
    for (j = 0; j < nj; j++)
      B[i][j] = (double) (i*(j+2) % nj) / nj;
}


void print_array(int ni, int nj,
		 double C[ 20 + 0][25 + 0])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "C");
  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++) {
	if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", C[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "C");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int ni = 20;
  int nj = 25;
  int nk = 30;


  double alpha;
  double beta;
  double C[ 20 + 0][25 + 0];
  double A[ 20 + 0][30 + 0];
  double B[ 30 + 0][25 + 0];


  init_array (ni, nj, nk, &alpha, &beta,
	      C,
	      A,
	      B);


  kernel_gemm (
	       alpha, beta,
	       C,
	       A,
	       B);


  print_array(ni, nj,  C);


  return 0;
}