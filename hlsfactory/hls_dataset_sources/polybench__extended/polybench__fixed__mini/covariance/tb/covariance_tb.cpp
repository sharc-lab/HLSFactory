#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "covariance.h"


t_ap_fixed data[ 32 + 0][28 + 0];
t_ap_fixed cov[ 28 + 0][28 + 0];
t_ap_fixed mean[ 28 + 0];


void init_array (int m, int n,
		 t_ap_fixed *float_n,
		 t_ap_fixed data[ 32 + 0][28 + 0])
{
  int i, j;

  *float_n = (t_ap_fixed)n;

  for (i = 0; i < 32; i++)
    for (j = 0; j < 28; j++)
      data[i][j] = t_ap_fixed(i*j) / t_ap_fixed(28.0);
}


void print_array(int m,
		 t_ap_fixed cov[ 28 + 0][28 + 0])

{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "cov");
  for (i = 0; i < m; i++)
    for (j = 0; j < m; j++) {
      if ((i * m + j) % 20 == 0) fprintf (stderr, "\n");
      fprintf (stderr, "%0.6lf ", (float)cov[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "cov");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int n = 32;
  int m = 28;


  t_ap_fixed float_n;
  
  
  init_array (m, n, &float_n, data);


  kernel_covariance ( float_n,
		     data,
		     cov,
		     mean);


  print_array(m, cov);


  return 0;
}