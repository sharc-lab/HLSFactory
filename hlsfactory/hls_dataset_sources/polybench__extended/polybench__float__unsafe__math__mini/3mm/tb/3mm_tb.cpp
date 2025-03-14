#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "3mm.h"


void init_array(int ni, int nj, int nk, int nl, int nm,
		double A[ 16 + 0][20 + 0],
		double B[ 20 + 0][18 + 0],
		double C[ 18 + 0][24 + 0],
		double D[ 24 + 0][22 + 0])
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nk; j++)
      A[i][j] = (double) ((i*j+1) % ni) / (5*ni);
  for (i = 0; i < nk; i++)
    for (j = 0; j < nj; j++)
      B[i][j] = (double) ((i*(j+1)+2) % nj) / (5*nj);
  for (i = 0; i < nj; i++)
    for (j = 0; j < nm; j++)
      C[i][j] = (double) (i*(j+3) % nl) / (5*nl);
  for (i = 0; i < nm; i++)
    for (j = 0; j < nl; j++)
      D[i][j] = (double) ((i*(j+2)+2) % nk) / (5*nk);
}


void print_array(int ni, int nl,
		 double G[ 16 + 0][22 + 0])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "G");
  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++) {
	if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", G[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "G");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int ni = 16;
  int nj = 18;
  int nk = 20;
  int nl = 22;
  int nm = 24;


   double E[ 16 + 0][18 + 0];
   double A[ 16 + 0][20 + 0];
   double B[ 20 + 0][18 + 0];
   double F[ 18 + 0][22 + 0];
   double C[ 18 + 0][24 + 0];
   double D[ 24 + 0][22 + 0];
   double G[ 16 + 0][22 + 0];


  init_array (ni, nj, nk, nl, nm,
	      A,
	      B,
	      C,
	      D);


  kernel_3mm (
	      E,
	      A,
	      B,
	      F,
	      C,
	      D,
	      G);


  print_array(ni, nl,  G);


  return 0;
}