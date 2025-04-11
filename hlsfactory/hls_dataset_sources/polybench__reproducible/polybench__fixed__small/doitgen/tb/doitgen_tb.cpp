#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "doitgen.h"


t_ap_fixed A[ 25 + 0][20 + 0][30 + 0];
t_ap_fixed sum[ 30 + 0];
t_ap_fixed C4[ 30 + 0][30 + 0];


void init_array(int nr, int nq, int np,
		t_ap_fixed A[ 25 + 0][20 + 0][30 + 0],
		t_ap_fixed C4[ 30 + 0][30 + 0])
{
  int i, j, k;

  for (i = 0; i < nr; i++)
    for (j = 0; j < nq; j++)
      for (k = 0; k < np; k++)
	A[i][j][k] = (t_ap_fixed) ((i*j + k)%np) / np;
  for (i = 0; i < np; i++)
    for (j = 0; j < np; j++)
      C4[i][j] = (t_ap_fixed) (i*j % np) / np;
}


void print_array(int nr, int nq, int np,
		 t_ap_fixed A[ 25 + 0][20 + 0][30 + 0])
{
  int i, j, k;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "A");
  for (i = 0; i < nr; i++)
    for (j = 0; j < nq; j++)
      for (k = 0; k < np; k++) {
	if ((i*nq*np+j*np+k) % 20 == 0) fprintf (stderr, "\n");
	fprintf (stderr, "%0.6lf ", (float)A[i][j][k]);
      }
  fprintf(stderr, "\nend   dump: %s\n", "A");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}


int main(int argc, char** argv)
{

  int nr = 25;
  int nq = 20;
  int np = 30;


  init_array (nr, nq, np,
	      A,
	      C4);


  kernel_doitgen (
		  A,
		  C4,
		  sum);


  print_array(nr, nq, np,  A);


  return 0;
}