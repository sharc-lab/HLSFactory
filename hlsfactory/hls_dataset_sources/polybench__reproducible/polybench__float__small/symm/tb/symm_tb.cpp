#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "symm.h"


void init_array(int m, int n,
		double *alpha,
		double *beta,
		double C[ 60 + 0][80 + 0],
		double A[ 60 + 0][60 + 0],
		double B[ 60 + 0][80 + 0])
{
  int i, j;

  *alpha = 1.5;
  *beta = 1.2;
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++) {
      C[i][j] = (double) ((i+j) % 100) / m;
      B[i][j] = (double) ((n+i-j) % 100) / m;
    }
  for (i = 0; i < m; i++) {
    for (j = 0; j <=i; j++)
      A[i][j] = (double) ((i+j) % 100) / m;
    for (j = i+1; j < m; j++)
      A[i][j] = -999;
  }
}


void print_array(int m, int n,
		 double C[ 60 + 0][80 + 0])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "C");
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++) {
	if ((i * m + j) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", C[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "C");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int m = 60;
  int n = 80;


  double alpha;
  double beta;
  double C[ 60 + 0][80 + 0];
  double A[ 60 + 0][60 + 0];
  double B[ 60 + 0][80 + 0];


  init_array (m, n, &alpha, &beta,
	      C,
	      A,
	      B);


  kernel_symm (
	       alpha, beta,
	       C,
	       A,
	       B);


  print_array(m, n, C);


  return 0;
}