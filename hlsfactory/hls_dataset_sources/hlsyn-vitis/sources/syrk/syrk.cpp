#include "syrk.h"
void kernel_syrk(double alpha, double beta, double C[80][80],
                 double A[80][60]) {
  double C_buf[80][80];
  double A_buf[80][60];
  memcpy(C_buf, C, sizeof(double) * 80 * 80);
  memcpy(A_buf, A, sizeof(double) * 80 * 60);

  int i;
  int j;
  int k;
  // BLAS PARAMS
  // TRANS = 'N'
  // UPLO  = 'L'
  //  =>  Form  C := alpha*A*A**T + beta*C.
  // A is NxM
  // C is NxN

  L0: for (i = 0; i < 80; i++) {
#pragma HLS pipeline auto{__PIPE__L0}
#pragma HLS unroll factor=auto{__PARA__L0}
    L1: for (j = 0; j < 80; j++) {
#pragma HLS unroll factor=auto{__PARA__L1}

      if (j <= i) {
        C_buf[i][j] *= beta;
      }
    }

    L2: for (k = 0; k < 60; k++) {
#pragma HLS pipeline auto{__PIPE__L2}
#pragma HLS unroll factor=auto{__PARA__L2}
      L3: for (j = 0; j < 80; j++) {
#pragma HLS unroll factor=auto{__PARA__L3}

        if (j <= i) {
          C_buf[i][j] += alpha * A_buf[i][k] * A_buf[j][k];
        }
      }
    }
  }
  memcpy(C, C_buf, sizeof(double) * 80 * 80);
}
