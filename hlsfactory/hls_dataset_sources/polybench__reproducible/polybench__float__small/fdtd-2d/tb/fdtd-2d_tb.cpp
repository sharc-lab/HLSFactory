#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "fdtd-2d.h"


void init_array (int tmax,
		 int nx,
		 int ny,
		 double ex[ 60 + 0][80 + 0],
		 double ey[ 60 + 0][80 + 0],
		 double hz[ 60 + 0][80 + 0],
		 double _fict_[ 40 + 0])
{
  int i, j;

  for (i = 0; i < tmax; i++)
    _fict_[i] = (double) i;
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++)
      {
	ex[i][j] = ((double) i*(j+1)) / nx;
	ey[i][j] = ((double) i*(j+2)) / ny;
	hz[i][j] = ((double) i*(j+3)) / nx;
      }
}


void print_array(int nx,
		 int ny,
		 double ex[ 60 + 0][80 + 0],
		 double ey[ 60 + 0][80 + 0],
		 double hz[ 60 + 0][80 + 0])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "ex");
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++) {
      if ((i * nx + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.6lf ", ex[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "ex");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");

  fprintf(stderr, "begin dump: %s", "ey");
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++) {
      if ((i * nx + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.6lf ", ey[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "ey");

  fprintf(stderr, "begin dump: %s", "hz");
  for (i = 0; i < nx; i++)
    for (j = 0; j < ny; j++) {
      if ((i * nx + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.6lf ", hz[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "hz");
}


int main(int argc, char** argv)
{

  int tmax = 40;
  int nx = 60;
  int ny = 80;


  double ex[ 60 + 0][80 + 0];
  double ey[ 60 + 0][80 + 0];
  double hz[ 60 + 0][80 + 0];
  double _fict_[ 40 + 0];


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