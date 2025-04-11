#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "trmm.h"


t_ap_fixed A[ 60 + 0][60 + 0];
t_ap_fixed B[ 60 + 0][80 + 0];


void init_array(int m, int n,
		t_ap_fixed *alpha,
		t_ap_fixed A[ 60 + 0][60 + 0],
		t_ap_fixed B[ 60 + 0][80 + 0])
{
  int i, j;

  *alpha = (t_ap_fixed(1.5));
  for (i = 0; i < m; i++) {
    for (j = 0; j < i; j++) {
      A[i][j] = (t_ap_fixed)((i+j) % m)/m;
    }
    A[i][i] = (t_ap_fixed(1.0));
    for (j = 0; j < n; j++) {
      B[i][j] = (t_ap_fixed)((n+(i-j)) % n)/n;
    }
 }

}


void print_array(int m, int n,
		 t_ap_fixed B[ 60 + 0][80 + 0])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "B");
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++) {
	if ((i * m + j) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", (float)B[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "B");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int m = 60;
  int n = 80;


  t_ap_fixed alpha;
  
  
  init_array (m, n, &alpha, A, B);


  kernel_trmm (alpha, A, B);


  print_array(m, n, B);


  return 0;
}