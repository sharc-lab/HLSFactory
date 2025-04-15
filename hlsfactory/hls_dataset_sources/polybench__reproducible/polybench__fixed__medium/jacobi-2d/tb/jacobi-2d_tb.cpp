#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "jacobi-2d.h"


t_ap_fixed A[ 250 + 0][250 + 0];
t_ap_fixed B[ 250 + 0][250 + 0];


void init_array (int n,
		 t_ap_fixed A[ 250 + 0][250 + 0],
		 t_ap_fixed B[ 250 + 0][250 + 0])
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      {
	A[i][j] = ((t_ap_fixed) i*(j+2) + 2) / n;
	B[i][j] = ((t_ap_fixed) i*(j+3) + 3) / n;
      }
}


void print_array(int n,
		 t_ap_fixed A[ 250 + 0][250 + 0])

{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "A");
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      if ((i * n + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.6lf ", (float)A[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "A");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int n = 250;
  int tsteps = 100;


  init_array (n, A, B);


  kernel_jacobi_2d( A, B);


  print_array(n, A);


  return 0;
}