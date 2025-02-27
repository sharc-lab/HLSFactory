#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "2mm.h"


t_ap_fixed tmp[ 180 + 0][190 + 0];
t_ap_fixed A[ 180 + 0][210 + 0];
t_ap_fixed B[ 210 + 0][190 + 0];
t_ap_fixed C[ 190 + 0][220 + 0];
t_ap_fixed D[ 180 + 0][220 + 0];


void init_array(int ni, int nj, int nk, int nl,
		t_ap_fixed *alpha,
		t_ap_fixed *beta,
		t_ap_fixed A[ 180 + 0][210 + 0],
		t_ap_fixed B[ 210 + 0][190 + 0],
		t_ap_fixed C[ 190 + 0][220 + 0],
		t_ap_fixed D[ 180 + 0][220 + 0])
{
  int i, j;

  *alpha = (t_ap_fixed(1.5));
  *beta = (t_ap_fixed(1.2));
  for (i = 0; i < ni; i++)
    for (j = 0; j < nk; j++)
      A[i][j] = (t_ap_fixed) ((i*j+1) % ni) / ni;
  for (i = 0; i < nk; i++)
    for (j = 0; j < nj; j++)
      B[i][j] = (t_ap_fixed) (i*(j+1) % nj) / nj;
  for (i = 0; i < nj; i++)
    for (j = 0; j < nl; j++)
      C[i][j] = (t_ap_fixed) ((i*(j+3)+1) % nl) / nl;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++)
      D[i][j] = (t_ap_fixed) (i*(j+2) % nk) / nk;
}


void print_array(int ni, int nl,
		 t_ap_fixed D[ 180 + 0][220 + 0])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "D");
  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++) {
	if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", (float)D[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "D");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int ni = 180;
  int nj = 190;
  int nk = 210;
  int nl = 220;


  t_ap_fixed alpha;
  t_ap_fixed beta;
  
  
  init_array (ni, nj, nk, nl, &alpha, &beta,
	      A,
	      B,
	      C,
	      D);


  kernel_2mm (
	      alpha, beta,
	      tmp,
	      A,
	      B,
	      C,
	      D);


  print_array(ni, nl,  D);


  return 0;
}