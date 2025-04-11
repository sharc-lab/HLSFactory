#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "syrk.h"


void init_array(int n, int m,
		double *alpha,
		double *beta,
		double C[ 30 + 0][30 + 0],
		double A[ 30 + 0][20 + 0])
{
  int i, j;

  *alpha = 1.5;
  *beta = 1.2;
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      A[i][j] = (double) ((i*j+1)%n) / n;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      C[i][j] = (double) ((i*j+2)%m) / m;
}


void print_array(int n,
		 double C[ 30 + 0][30 + 0])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "C");
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
	if ((i * n + j) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", C[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "C");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int n = 30;
  int m = 20;


  double alpha;
  double beta;
  double C[ 30 + 0][30 + 0];
  double A[ 30 + 0][20 + 0];


  init_array (n, m, &alpha, &beta, C, A);


  kernel_syrk (alpha, beta, C, A);


  print_array(n, C);


  return 0;
}