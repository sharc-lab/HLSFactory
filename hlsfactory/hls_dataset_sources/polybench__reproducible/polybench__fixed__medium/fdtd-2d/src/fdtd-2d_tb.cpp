#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "fdtd-2d.h"


t_ap_fixed ex[ 200 + 0][240 + 0];
t_ap_fixed ey[ 200 + 0][240 + 0];
t_ap_fixed hz[ 200 + 0][240 + 0];
t_ap_fixed _fict_[ 100 + 0];


void init_array (int tmax,
		 int nx,
		 int ny,
		 t_ap_fixed ex[ 200 + 0][240 + 0],
		 t_ap_fixed ey[ 200 + 0][240 + 0],
		 t_ap_fixed hz[ 200 + 0][240 + 0],
		 t_ap_fixed _fict_[ 100 + 0])
{
  int i, j;

  for (i = 0; i < tmax; i++)
    _fict_[i] = (t_ap_fixed) i;
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++)
      {
	ex[i][j] = ((t_ap_fixed) i*(j+1)) / nx;
	ey[i][j] = ((t_ap_fixed) i*(j+2)) / ny;
	hz[i][j] = ((t_ap_fixed) i*(j+3)) / nx;
      }
}


void print_array(int nx,
		 int ny,
		 t_ap_fixed ex[ 200 + 0][240 + 0],
		 t_ap_fixed ey[ 200 + 0][240 + 0],
		 t_ap_fixed hz[ 200 + 0][240 + 0])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "ex");
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++) {
      if ((i * nx + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.6lf ", (float)ex[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "ex");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");

  fprintf(stderr, "begin dump: %s", "ey");
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++) {
      if ((i * nx + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.6lf ", (float)ey[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "ey");

  fprintf(stderr, "begin dump: %s", "hz");
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++) {
      if ((i * nx + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.6lf ", (float)hz[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "hz");
}


int main(int argc, char** argv)
{

  int tmax = 100;
  int nx = 200;
  int ny = 240;


  init_array (tmax, nx, ny,
	      ex,
	      ey,
	      hz,
	      _fict_);


  kernel_fdtd_2d (
		  ex,
		  ey,
		  hz,
		  _fict_);


  print_array(nx, ny, ex,
				    ey,
				    hz);


  return 0;
}