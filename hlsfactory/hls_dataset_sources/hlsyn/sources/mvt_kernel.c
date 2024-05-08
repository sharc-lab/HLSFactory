#pragma ACCEL kernel

void kernel_mvt(double x1[120], double x2[120], double y_1[120], double y_2[120], double A[120][120])
{
  int i;
  int j;
//#pragma scop
  
#pragma ACCEL PIPELINE auto{__PIPE__L0}
  
#pragma ACCEL TILE FACTOR=auto{__TILE__L0}
  
#pragma ACCEL PARALLEL FACTOR=auto{__PARA__L0}
  for (i = 0; i < 120; i++) {
    
#pragma ACCEL PARALLEL reduction = x1 FACTOR=auto{__PARA__L2}
    for (j = 0; j < 120; j++) {
      x1[i] += A[i][j] * y_1[j];
    }
  }
  
#pragma ACCEL PIPELINE auto{__PIPE__L1}
  
#pragma ACCEL TILE FACTOR=auto{__TILE__L1}
  
#pragma ACCEL PARALLEL FACTOR=auto{__PARA__L1}
  for (i = 0; i < 120; i++) {
    
#pragma ACCEL PARALLEL reduction = x2 FACTOR=auto{__PARA__L3}
    for (j = 0; j < 120; j++) {
      x2[i] += A[j][i] * y_2[j];
    }
  }
//#pragma endscop
}
