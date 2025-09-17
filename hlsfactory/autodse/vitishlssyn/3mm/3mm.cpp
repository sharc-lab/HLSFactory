#include "3mm.h"
#include <string.h>
void kernel_3mm(int ni,int nj,int nk,int nl,int nm,double E[40][50],double A[40][60],double B[60][50],double F[50][70],double C[50][80],double D[80][70],double G[40][70])
{
  double E_buf[40][50];
  double A_buf[40][60];
  double B_buf[60][50];
  double F_buf[50][70];
  double C_buf[50][80];
  double D_buf[80][70];
  double G_buf[40][70];
  int i;
  int j;
  int k;
//#pragma scop
/* E_buf := A_buf*B_buf */
  memcpy(C_buf, C, sizeof(double) * 50 * 80);
  memcpy(B_buf, B, sizeof(double) * 60 * 50);
  memcpy(A_buf, A, sizeof(double) * 40 * 60);
  memcpy(D_buf, D, sizeof(double) * 80 * 70);
L0:   for (i = 0; i < 40; i++) {

#pragma HLS pipeline auto{__PIPE__L0}

#pragma HLS unroll factor=auto{__PARA__L0}

L3:     for (j = 0; j < 50; j++) {

#pragma HLS pipeline auto{__PIPE__L3}

#pragma HLS unroll factor=auto{__PARA__L3}

      E_buf[i][j] = 0.0;
L6:       for (k = 0; k < 60; ++k) {

#pragma HLS pipeline auto{__PIPE__L6}

#pragma HLS unroll factor=auto{__PARA__L6}

        E_buf[i][j] += A_buf[i][k] * B_buf[k][j];
      }
    }
  }
/* F_buf := C_buf*D_buf */
L1:   for (i = 0; i < 50; i++) {

#pragma HLS pipeline auto{__PIPE__L1}

#pragma HLS unroll factor=auto{__PARA__L1}

L4:     for (j = 0; j < 70; j++) {

#pragma HLS pipeline auto{__PIPE__L4}

#pragma HLS unroll factor=auto{__PARA__L4}

      F_buf[i][j] = 0.0;
L7:       for (k = 0; k < 80; ++k) {

#pragma HLS pipeline auto{__PIPE__L7}

#pragma HLS unroll factor=auto{__PARA__L7}

        F_buf[i][j] += C_buf[i][k] * D_buf[k][j];
      }
    }
  }
/* G_buf := E_buf*F_buf */
L2:   for (i = 0; i < 40; i++) {

#pragma HLS pipeline auto{__PIPE__L2}

#pragma HLS unroll factor=auto{__PARA__L2}

L5:     for (j = 0; j < 70; j++) {

#pragma HLS pipeline auto{__PIPE__L5}

#pragma HLS unroll factor=auto{__PARA__L5}

      G_buf[i][j] = 0.0;
L8:       for (k = 0; k < 50; ++k) {

#pragma HLS pipeline auto{__PIPE__L8}

#pragma HLS unroll factor=auto{__PARA__L8}

        G_buf[i][j] += E_buf[i][k] * F_buf[k][j];
      }
    }
  }
//#pragma endscop
  memcpy(F, F_buf, sizeof(double) * 50 * 70);
  memcpy(E, E_buf, sizeof(double) * 40 * 50);
  memcpy(G, G_buf, sizeof(double) * 40 * 70);
}
