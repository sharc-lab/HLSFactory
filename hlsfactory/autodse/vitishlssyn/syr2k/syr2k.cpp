#include "syr2k.h"
#include <string.h>
void kernel_syr2k(double alpha,double beta,double C[80][80],double A[80][60],double B[80][60])
{
  double C_buf[80][80];
  double A_buf[80][60];
  double B_buf[80][60];
  int i;
  int j;
  int k;
//BLAS PARAMS
//UPLO  = 'L'
//TRA_bufNS = 'N'
//A is NxM
//B is NxM
//C is NxN
  memcpy(A_buf, A, sizeof(double) * 80 * 60);
  memcpy(B_buf, B, sizeof(double) * 80 * 60);
  memcpy(C_buf, C, sizeof(double) * 80 * 80);
L0:   for (i = 0; i < 80; i++) {

#pragma HLS pipeline auto{__PIPE__L0}

#pragma HLS unroll factor=auto{__PARA__L0}

L1:     for (j = 0; j < 80; j++) {

#pragma HLS pipeline auto{__PIPE__L1}

#pragma HLS unroll factor=auto{__PARA__L1}

      if (j <= i) {
        C_buf[i][j] *= beta;
      }
    }
L2:     for (k = 0; k < 60; k++) {

#pragma HLS pipeline auto{__PIPE__L2}

#pragma HLS unroll factor=auto{__PARA__L2}

L3:       for (j = 0; j < 80; j++) {

#pragma HLS pipeline auto{__PIPE__L3}

#pragma HLS unroll factor=auto{__PARA__L3}

        if (j <= i) {
          C_buf[i][j] += A_buf[j][k] * alpha * B_buf[i][k] + B_buf[j][k] * alpha * A_buf[i][k];
        }
      }
    }
  }
  memcpy(C, C_buf, sizeof(double) * 80 * 80);
}
