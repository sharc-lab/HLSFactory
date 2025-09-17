#include "jacobi-2d.h"
#include <string.h>
void kernel_jacobi_2d(int tsteps,int n,double A[90][90],double B[90][90])
{
  double A_buf[90][90];
  double B_buf[90][90];
  int t;
  int i;
  int j;
//#pragma scop
  memcpy(A_buf, A, sizeof(double) * 90 * 90);
  memcpy(B_buf, B, sizeof(double) * 90 * 90);
L0:   for (t = 0; t < 40; t++) {

#pragma HLS pipeline auto{__PIPE__L0}

#pragma HLS unroll factor=auto{__PARA__L0}

L1:     for (i = 1; i < 89; i++) {

#pragma HLS pipeline auto{__PIPE__L1}

#pragma HLS unroll factor=auto{__PARA__L1}

L3:       for (j = 1; j < 89; j++) {

#pragma HLS pipeline auto{__PIPE__L3}

#pragma HLS unroll factor=auto{__PARA__L3}

        B_buf[i][j] = 0.2 * (A_buf[i][j] + A_buf[i][j - 1] + A_buf[i][1 + j] + A_buf[1 + i][j] + A_buf[i - 1][j]);
      }
    }
L2:     for (i = 1; i < 89; i++) {

#pragma HLS pipeline auto{__PIPE__L2}

#pragma HLS unroll factor=auto{__PARA__L2}

L4:       for (j = 1; j < 89; j++) {

#pragma HLS pipeline auto{__PIPE__L4}

#pragma HLS unroll factor=auto{__PARA__L4}

        A_buf[i][j] = 0.2 * (B_buf[i][j] + B_buf[i][j - 1] + B_buf[i][1 + j] + B_buf[1 + i][j] + B_buf[i - 1][j]);
      }
    }
  }
//#pragma endscop
  memcpy(B, B_buf, sizeof(double) * 90 * 90);
  memcpy(A, A_buf, sizeof(double) * 90 * 90);
}
