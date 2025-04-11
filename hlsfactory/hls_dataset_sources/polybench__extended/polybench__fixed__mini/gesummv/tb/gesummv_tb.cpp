#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "gesummv.h"


t_ap_fixed A[ 30 + 0][30 + 0];
t_ap_fixed B[ 30 + 0][30 + 0];
t_ap_fixed tmp[ 30 + 0];
t_ap_fixed x[ 30 + 0];
t_ap_fixed y[ 30 + 0];


void init_array(int n,
		t_ap_fixed *alpha,
		t_ap_fixed *beta,
		t_ap_fixed A[ 30 + 0][30 + 0],
		t_ap_fixed B[ 30 + 0][30 + 0],
		t_ap_fixed x[ 30 + 0])
{
  int i, j;

  *alpha = (t_ap_fixed(1.5));
  *beta = (t_ap_fixed(1.2));
  for (i = 0; i < n; i++)
    {
      x[i] = (t_ap_fixed)( i % n) / n;
      for (j = 0; j < n; j++) {
	A[i][j] = (t_ap_fixed) ((i*j+1) % n) / n;
	B[i][j] = (t_ap_fixed) ((i*j+2) % n) / n;
      }
    }
}


void print_array(int n,
		 t_ap_fixed y[ 30 + 0])

{
  int i;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "y");
  for (i = 0; i < n; i++) {
    if (i % 20 == 0) fprintf (stderr, "\n");
    fprintf (stderr, "%0.6lf ", (float)y[i]);
  }
  fprintf(stderr, "\nend   dump: %s\n", "y");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int n = 30;


  t_ap_fixed alpha;
  t_ap_fixed beta;
   
   
  init_array (n, &alpha, &beta,
	      A,
	      B,
	      x);


  kernel_gesummv (alpha, beta,
		  A,
		  B,
		  tmp,
		  x,
		  y);


  print_array(n, y);


  return 0;
}