#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "jacobi-1d.h"


t_ap_fixed A[ 30 + 0];
t_ap_fixed B[ 30 + 0];


void init_array (int n,
		 t_ap_fixed A[ 30 + 0],
		 t_ap_fixed B[ 30 + 0])
{
  int i;

  for (i = 0; i < n; i++)
      {
	A[i] = ((t_ap_fixed) i+ 2) / n;
	B[i] = ((t_ap_fixed) i+ 3) / n;
      }
}


void print_array(int n,
		 t_ap_fixed A[ 30 + 0])

{
  int i;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "A");
  for (i = 0; i < n; i++)
    {
      if (i % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.6lf ", (float)A[i]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "A");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int n = 30;
  int tsteps = 20;


  init_array (n, A, B);


  kernel_jacobi_1d( A, B);


  print_array(n, A);


  return 0;
}