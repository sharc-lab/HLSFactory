#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "atax.h"


t_ap_fixed A[ 116 + 0][124 + 0];
t_ap_fixed x[ 124 + 0];
t_ap_fixed y[ 124 + 0];
t_ap_fixed tmp[ 116 + 0];


void init_array (int m, int n,
		 t_ap_fixed A[ 116 + 0][124 + 0],
		 t_ap_fixed x[ 124 + 0])
{
  int i, j;
  t_ap_fixed fn;
  fn = (t_ap_fixed)n;

  for (i = 0; i < n; i++)
      x[i] = (t_ap_fixed(1.0)) + ( (t_ap_fixed(i)) / fn );
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
      A[i][j] = (t_ap_fixed) (t_ap_fixed(((i+j) % n))) / (t_ap_fixed(5.0)*t_ap_fixed(m));
}


void print_array(int n,
		 t_ap_fixed y[ 124 + 0])

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

  int m = 116;
  int n = 124;


  init_array (m, n, A, x);


  kernel_atax (
	       A,
	       x,
	       y,
	       tmp);


  print_array(n, y);


  return 0;
}