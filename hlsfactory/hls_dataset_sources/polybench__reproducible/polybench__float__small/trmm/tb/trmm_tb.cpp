#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "trmm.h"


void init_array(int m, int n,
		double *alpha,
		double A[ 60 + 0][60 + 0],
		double B[ 60 + 0][80 + 0])
{
  int i, j;

  *alpha = 1.5;
  for (i = 0; i < m; i++) {
    for (j = 0; j < i; j++) {
      A[i][j] = (double)((i+j) % m)/m;
    }
    A[i][i] = 1.0;
    for (j = 0; j < n; j++) {
      B[i][j] = (double)((n+(i-j)) % n)/n;
    }
 }

}


void print_array(int m, int n,
		 double B[ 60 + 0][80 + 0])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "B");
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++) {
	if ((i * m + j) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", B[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "B");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int m = 60;
  int n = 80;


  double alpha;
  double A[ 60 + 0][60 + 0];
  double B[ 60 + 0][80 + 0];


  init_array (m, n, &alpha, A, B);


  kernel_trmm (alpha, A, B);


  print_array(m, n, B);


  return 0;
}