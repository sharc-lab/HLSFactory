#include "trmm-opt.h"
#include <string.h>
void kernel_trmm(double alpha,double A[60][60],double B[60][80])
{
  double A_buf[60][60];
  double B_buf[60][80];
  memcpy(A_buf, A, sizeof(double) * 60 * 60);
  memcpy(B_buf, B, sizeof(double) * 60 * 80);
L0:   for (int i = 0; i < 60; i++) {

#pragma HLS pipeline auto{__PIPE__L0}

#pragma HLS unroll factor=auto{__PARA__L0}

L1:     for (int j = 0; j < 80; j++) {

#pragma HLS pipeline auto{__PIPE__L1}

#pragma HLS unroll factor=auto{__PARA__L1}

      double sum = B_buf[i][j];
L2:       for (int k = 0; k < 60; k++) {

#pragma HLS pipeline auto{__PIPE__L2}

#pragma HLS unroll factor=auto{__PARA__L2}

        if (k > i) {
          sum += A_buf[k][i] * B_buf[k][j];
        }
      }
      B_buf[i][j] = alpha * sum;
    }
  }
  memcpy(B, B_buf, sizeof(double) * 60 * 80);
}
