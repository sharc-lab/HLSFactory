#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "trisolv.h"


void init_array(int n,
		double L[ 120 + 0][120 + 0],
		double x[ 120 + 0],
		double b[ 120 + 0])
{
  int i, j;

  for (i = 0; i < n; i++)
    {
      x[i] = - 999;
      b[i] =  i ;
      for (j = 0; j <= i; j++)
	L[i][j] = (double) (i+n-j+1)*2/n;
    }
}


void print_array(int n,
		 double x[ 120 + 0])

{
  int i;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "x");
  for (i = 0; i < n; i++) {
    fprintf (stderr, "%0.6lf ", x[i]);
    if (i % 20 == 0) fprintf (stderr, "\n");
  }
  fprintf(stderr, "\nend   dump: %s\n", "x");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int n = 120;


   double L[ 120 + 0][120 + 0];
   double x[ 120 + 0];
   double b[ 120 + 0];


  init_array (n, L, x, b);


  kernel_trisolv (L, x, b);


  print_array(n, x);


  return 0;
}