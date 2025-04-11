#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "gemver.h"


t_ap_fixed A[ 120 + 0][120 + 0];
t_ap_fixed u1[ 120 + 0];
t_ap_fixed v1[ 120 + 0];
t_ap_fixed u2[ 120 + 0];
t_ap_fixed v2[ 120 + 0];
t_ap_fixed w[ 120 + 0];
t_ap_fixed x[ 120 + 0];
t_ap_fixed y[ 120 + 0];
t_ap_fixed z[ 120 + 0];


void init_array (int n,
		 t_ap_fixed *alpha,
		 t_ap_fixed *beta,
		 t_ap_fixed A[ 120 + 0][120 + 0],
		 t_ap_fixed u1[ 120 + 0],
		 t_ap_fixed v1[ 120 + 0],
		 t_ap_fixed u2[ 120 + 0],
		 t_ap_fixed v2[ 120 + 0],
		 t_ap_fixed w[ 120 + 0],
		 t_ap_fixed x[ 120 + 0],
		 t_ap_fixed y[ 120 + 0],
		 t_ap_fixed z[ 120 + 0])
{
  int i, j;

  *alpha = (t_ap_fixed(1.5));
  *beta = (t_ap_fixed(1.2));

  t_ap_fixed fn = (t_ap_fixed)n;

  for (i = 0; i < n; i++)
    {
      u1[i] = i;
      u2[i] = (t_ap_fixed(i+1)) / (fn)/(t_ap_fixed(2.0));
      v1[i] = (t_ap_fixed(i+1)) / (fn)/(t_ap_fixed(4.0));
      v2[i] = (t_ap_fixed(i+1)) / (fn)/(t_ap_fixed(6.0));
      y[i] = (t_ap_fixed(i+1)) / (fn)/(t_ap_fixed(8.0));
      z[i] = (t_ap_fixed(i+1)) / (fn)/(t_ap_fixed(9.0));
      x[i] = (t_ap_fixed(0.0));
      w[i] = (t_ap_fixed(0.0));
      for (j = 0; j < n; j++)
        A[i][j] = (t_ap_fixed) ((t_ap_fixed(i*j % n)) / (t_ap_fixed(n)));
    }
}


void print_array(int n,
		 t_ap_fixed w[ 120 + 0])
{
  int i;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "w");
  for (i = 0; i < n; i++) {
    if (i % 20 == 0) fprintf (stderr, "\n");
    fprintf (stderr, "%0.6lf ", (float)w[i]);
  }
  fprintf(stderr, "\nend   dump: %s\n", "w");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int n = 120;


  t_ap_fixed alpha;
  t_ap_fixed beta;
   
   
  init_array (n, &alpha, &beta,
	      A,
	      u1,
	      v1,
	      u2,
	      v2,
	      w,
	      x,
	      y,
	      z);


  kernel_gemver (alpha, beta,
		 A,
		 u1,
		 v1,
		 u2,
		 v2,
		 w,
		 x,
		 y,
		 z);


  print_array(n, w);


  return 0;
}