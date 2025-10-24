#include "doitgen-red.h"
void kernel_doitgen(double A[25][20][30], double C4[30][30], double sum[30]) {
  double A_buf[25][20][30];
  double C4_buf[30][30];
  double sum_buf[30];
  memcpy(A_buf, A, sizeof(double) * 25 * 20 * 30);
  memcpy(C4_buf, C4, sizeof(double) * 30 * 30);

  int r;
  int q;
  int p;
  int s;
  //#pragma scop

  L0: for (r = 0; r < 25; r++) {
#pragma HLS pipeline auto{__PIPE__L0}
#pragma HLS unroll factor=auto{__PARA__L0}

    L1: for (q = 0; q < 20; q++) {
#pragma HLS pipeline auto{__PIPE__L1}
#pragma HLS unroll factor=auto{__PARA__L1}

      L2: for (p = 0; p < 30; p++) {
#pragma HLS pipeline auto{__PIPE__L2}
#pragma HLS unroll factor=auto{__PARA__L2}

        double sum_tmp = 0.0;
        L3: for (s = 0; s < 30; s++) {
#pragma HLS unroll factor=auto{__PARA__L3}

          sum_tmp += A_buf[r][q][s] * C4_buf[s][p];
        }
        sum_buf[p] = sum_tmp;
      }
      L4: for (p = 0; p < 30; p++) {
        A_buf[r][q][p] = sum_buf[p];
      }
    }
  }
  //#pragma endscop
  memcpy(A, A_buf, sizeof(double) * 25 * 20 * 30);
  memcpy(sum, sum_buf, sizeof(double) * 30);
}
