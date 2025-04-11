#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "gramschmidt.h"


void init_array(int m, int n,
		double A[ 60 + 0][80 + 0],
		double R[ 80 + 0][80 + 0],
		double Q[ 60 + 0][80 + 0])
{
  int i, j;

  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++) {
      A[i][j] = (((double) ((i*j) % m) / m )*100) + 10;
      Q[i][j] = 0.0;
    }
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      R[i][j] = 0.0;
}


void print_array(int m, int n,
		 double A[ 60 + 0][80 + 0],
		 double R[ 80 + 0][80 + 0],
		 double Q[ 60 + 0][80 + 0])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "R");
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
	if ((i*n+j) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", R[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "R");

  fprintf(stderr, "begin dump: %s", "Q");
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++) {
	if ((i*n+j) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", Q[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "Q");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int m = 60;
  int n = 80;


  double A[ 60 + 0][80 + 0];
  double R[ 80 + 0][80 + 0];
  double Q[ 60 + 0][80 + 0];


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