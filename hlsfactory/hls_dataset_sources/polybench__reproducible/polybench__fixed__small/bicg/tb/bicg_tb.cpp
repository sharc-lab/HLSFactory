#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "bicg.h"


t_ap_fixed A[ 124 + 0][116 + 0];
t_ap_fixed s[ 116 + 0];
t_ap_fixed q[ 124 + 0];
t_ap_fixed p[ 116 + 0];
t_ap_fixed r[ 124 + 0];


void init_array (int m, int n,
		 t_ap_fixed A[ 124 + 0][116 + 0],
		 t_ap_fixed r[ 124 + 0],
		 t_ap_fixed p[ 116 + 0])
{
  int i, j;

  for (i = 0; i < m; i++)
    p[i] = (t_ap_fixed)(i % m) / m;
  for (i = 0; i < n; i++) {
    r[i] = (t_ap_fixed)(i % n) / n;
    for (j = 0; j < m; j++)
      A[i][j] = (t_ap_fixed) (i*(j+1) % n)/n;
  }
}


void print_array(int m, int n,
		 t_ap_fixed s[ 116 + 0],
		 t_ap_fixed q[ 124 + 0])

{
  int i;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "s");
  for (i = 0; i < m; i++) {
    if (i % 20 == 0) fprintf (stderr, "\n");
    fprintf (stderr, "%0.6lf ", (float)s[i]);
  }
  fprintf(stderr, "\nend   dump: %s\n", "s");
  fprintf(stderr, "begin dump: %s", "q");
  for (i = 0; i < n; i++) {
    if (i % 20 == 0) fprintf (stderr, "\n");
    fprintf (stderr, "%0.6lf ", (float)q[i]);
  }
  fprintf(stderr, "\nend   dump: %s\n", "q");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int n = 124;
  int m = 116;


  init_array (m, n,
	      A,
	      r,
	      p);


  kernel_bicg (
	       A,
	       s,
	       q,
	       p,
	       r);


  print_array(m, n, s, q);


  return 0;
}