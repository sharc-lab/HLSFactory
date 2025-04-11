#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "durbin.h"


t_ap_fixed r[ 40 + 0];
t_ap_fixed y[ 40 + 0];


void init_array (int n,
		 t_ap_fixed r[ 40 + 0])
{
  int i, j;

  for (i = 0; i < n; i++)
    {
      r[i] = (n+1-i);
    }
}


void print_array(int n,
		 t_ap_fixed y[ 40 + 0])

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

  int n = 40;


  init_array (n, r);


  kernel_durbin (
		 r,
		 y);


  print_array(n, y);


  return 0;
}