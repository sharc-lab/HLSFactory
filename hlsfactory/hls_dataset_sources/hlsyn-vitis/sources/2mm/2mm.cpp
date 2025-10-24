#include "2mm.h"
void kernel_2mm(int ni, int nj, int nk, int nl, double alpha, double beta,
                double tmp[40][50], double A[40][70], double B[70][50],
                double C[50][80], double D[40][80]) {
  double tmp_buf[40][50];
  double A_buf[40][70];
  double B_buf[70][50];
  double C_buf[50][80];
  double D_buf[40][80];
  memcpy(A_buf, A, sizeof(double) * 40 * 70);
  memcpy(B_buf, B, sizeof(double) * 70 * 50);
  memcpy(C_buf, C, sizeof(double) * 50 * 80);
  memcpy(D_buf, D, sizeof(double) * 40 * 80);

  int i;
  int j;
  int k;
  //#pragma scop
  /* D := alpha*A*B*C + beta*D */

  L0: for (i = 0; i < 40; i++) {
#pragma HLS pipeline auto{__PIPE__L0}
#pragma HLS unroll factor=auto{__PARA__L0}

    L1: for (j = 0; j < 50; j++) {
#pragma HLS pipeline auto{__PIPE__L2}
#pragma HLS unroll factor=auto{__PARA__L2}

      tmp_buf[i][j] = 0.0;
      L2: for (k = 0; k < 70; ++k) {
#pragma HLS unroll factor=auto{__PARA__L4}
        tmp_buf[i][j] += alpha * A_buf[i][k] * B_buf[k][j];
      }
    }
  }

  L3: for (i = 0; i < 40; i++) {
#pragma HLS pipeline auto{__PIPE__L1}
#pragma HLS unroll factor=auto{__PARA__L1}

    L4: for (j = 0; j < 80; j++) {
#pragma HLS pipeline auto{__PIPE__L3}
#pragma HLS unroll factor=auto{__PARA__L3}

      D_buf[i][j] *= beta;
      L5: for (k = 0; k < 50; ++k) {
#pragma HLS unroll factor=auto{__PARA__L5}
        D_buf[i][j] += tmp_buf[i][k] * C_buf[k][j];
      }
    }
  }
  //#pragma endscop
  memcpy(tmp, tmp_buf, sizeof(double) * 40 * 50);
  memcpy(D, D_buf, sizeof(double) * 40 * 80);
}
