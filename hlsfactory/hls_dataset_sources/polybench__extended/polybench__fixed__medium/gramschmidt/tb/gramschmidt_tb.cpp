#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "gramschmidt.h"


t_ap_fixed A[ 200 + 0][240 + 0];
t_ap_fixed R[ 240 + 0][240 + 0];
t_ap_fixed Q[ 200 + 0][240 + 0];


void init_array(int m, int n,
		t_ap_fixed A[ 200 + 0][240 + 0],
		t_ap_fixed R[ 240 + 0][240 + 0],
		t_ap_fixed Q[ 200 + 0][240 + 0])
{
  int i, j;

  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++) {
      A[i][j] = (( (t_ap_fixed(i*j % m)) / (t_ap_fixed(m)) ) * (t_ap_fixed(100.0)) ) + (t_ap_fixed(10.0));
      Q[i][j] = (t_ap_fixed(0.0));
    }
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      R[i][j] = (t_ap_fixed(0.0));
}


void print_array(int m, int n,
		 t_ap_fixed A[ 200 + 0][240 + 0],
		 t_ap_fixed R[ 240 + 0][240 + 0],
		 t_ap_fixed Q[ 200 + 0][240 + 0])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "R");
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
	if ((i*n+j) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", (float)R[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "R");

  fprintf(stderr, "begin dump: %s", "Q");
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++) {
	if ((i*n+j) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", (float)Q[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "Q");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int m = 200;
  int n = 240;


  init_array (m, n,
	      A,
	      R,
	      Q);


  kernel_gramschmidt (
		      A,
		      R,
		      Q);


  print_array(m, n, A, R, Q);


  return 0;
}