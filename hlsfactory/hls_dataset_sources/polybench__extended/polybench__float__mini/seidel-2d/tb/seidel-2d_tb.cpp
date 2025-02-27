#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "seidel-2d.h"


void init_array (int n,
		 double A[ 40 + 0][40 + 0])
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      A[i][j] = ((double) i*(j+2) + 2) / n;
}


void print_array(int n,
		 double A[ 40 + 0][40 + 0])

{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "A");
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      if ((i * n + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.6lf ", A[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "A");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int n = 40;
  int tsteps = 20;


   double A[ 40 + 0][40 + 0];


  init_array (n, A);


  kernel_seidel_2d ( A);


  print_array(n, A);


  return 0;
}