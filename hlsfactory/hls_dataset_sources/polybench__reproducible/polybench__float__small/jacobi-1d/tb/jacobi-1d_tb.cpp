#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "jacobi-1d.h"


void init_array (int n,
		 double A[ 120 + 0],
		 double B[ 120 + 0])
{
  int i;

  for (i = 0; i < n; i++)
      {
	A[i] = ((double) i+ 2) / n;
	B[i] = ((double) i+ 3) / n;
      }
}


void print_array(int n,
		 double A[ 120 + 0])

{
  int i;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "A");
  for (i = 0; i < n; i++)
    {
      if (i % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.6lf ", A[i]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "A");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int n = 120;
  int tsteps = 40;


   double A[ 120 + 0];
   double B[ 120 + 0];


  init_array (n, A, B);


  kernel_jacobi_1d( A, B);


  print_array(n, A);


  return 0;
}