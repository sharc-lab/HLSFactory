#include "bicg-large.h"
void kernel_bicg(int m, int n, double A[410][390], double s[390], double q[410],
                 double p[390], double r[410]) {
  double A_buf[410][390];
  double s_buf[390];
  double q_buf[410];
  double p_buf[390];
  double r_buf[410];
  memcpy(A_buf, A, sizeof(double) * 410 * 390);
  memcpy(p_buf, p, sizeof(double) * 390);
  memcpy(r_buf, r, sizeof(double) * 410);

  int i;
  int j;
  L0: for (i = 0; i < 390; i++)
    s_buf[i] = ((double)0);

  L1: for (i = 0; i < 410; i++) {
#pragma HLS pipeline auto{__PIPE__L0}
#pragma HLS unroll factor=auto{__PARA__L0}

    q_buf[i] = 0.0;

    L2: for (j = 0; j < 390; j++) {
      s_buf[j] += r_buf[i] * A_buf[i][j];
      q_buf[i] += A_buf[i][j] * p_buf[j];
    }
  }
  memcpy(s, s_buf, sizeof(double) * 390);
  memcpy(q, q_buf, sizeof(double) * 410);
}
