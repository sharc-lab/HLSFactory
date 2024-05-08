#pragma ACCEL kernel

void kernel_atax(int m,int n,double A[116][124],double x[124],double y[124],double tmp[116])
{
  int i;
  int j;
//#pragma scop
  for (i = 0; i < 124; i++) 
    y[i] = ((double )0);
  
#pragma ACCEL PIPELINE auto{__PIPE__L0}
  
#pragma ACCEL TILE FACTOR=auto{__TILE__L0}
  
#pragma ACCEL PARALLEL FACTOR=auto{__PARA__L0}
  for (i = 0; i < 116; i++) {
    tmp[i] = 0.0;
    
#pragma ACCEL PARALLEL reduction=tmp FACTOR=auto{__PARA__L0_0}
    for (j = 0; j < 124; j++) {
      tmp[i] += A[i][j] * x[j];
    }
    
#pragma ACCEL PARALLEL reduction=y FACTOR=auto{__PARA__L0_1}
    for (j = 0; j < 124; j++) {
      y[j] += A[i][j] * tmp[i];
    }
  }
//#pragma endscop
}
