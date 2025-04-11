#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "ludcmp.h"


void init_array (int n,
		 double A[ 40 + 0][40 + 0],
		 double b[ 40 + 0],
		 double x[ 40 + 0],
		 double y[ 40 + 0])
{
  int i, j;
  double fn = (double)n;

  for (i = 0; i < n; i++)
    {
      x[i] = 0;
      y[i] = 0;
      b[i] = (i+1)/fn/2.0 + 4;
    }

  for (i = 0; i < n; i++)
    {
      for (j = 0; j <= i; j++)
	A[i][j] = (double)(-j % n) / n + 1;
      for (j = i+1; j < n; j++) {
	A[i][j] = 0;
      }
      A[i][i] = 1;
    }


  int r,s,t;
   double B[ 40 + 0][40 + 0];
  for (r = 0; r < n; ++r)
    for (s = 0; s < n; ++s)
      B[r][s] = 0;
  for (t = 0; t < n; ++t)
    for (r = 0; r < n; ++r)
      for (s = 0; s < n; ++s)
	B[r][s] += A[r][t] * A[s][t];
    for (r = 0; r < n; ++r)
      for (s = 0; s < n; ++s)
	A[r][s] = B[r][s];

}


void print_array(int n,
		 double x[ 40 + 0])

{
  int i;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "x");
  for (i = 0; i < n; i++) {
    if (i % 20 == 0) fprintf (stderr, "\n");
    fprintf (stderr, "%0.6lf ", x[i]);
  }
  fprintf(stderr, "\nend   dump: %s\n", "x");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int n = 40;


   double A[ 40 + 0][40 + 0];
   double b[ 40 + 0];
   double x[ 40 + 0];
   double y[ 40 + 0];


  init_array (n,
	      A,
	      b,
	      x,
	      y);


  kernel_ludcmp (
		 A,
		 b,
		 x,
		 y);


  print_array(n, x);


  return 0;
}