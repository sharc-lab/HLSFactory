
#pragma ACCEL kernel

void kernel_atax(double A[390][410],double x[410],double y[410],double tmp[390])
{
  int i;
  int j;
  for (i = 0; i < 410; i++) 
    y[i] = ((double )0);
  
#pragma ACCEL PIPELINE auto{__PIPE__L0}
  
#pragma ACCEL TILE FACTOR=auto{__TILE__L0}
  
#pragma ACCEL PARALLEL FACTOR=auto{__PARA__L0}
  for (i = 0; i < 390; i++) {
    tmp[i] = 0.0;
    
#pragma ACCEL PARALLEL reduction=tmp FACTOR=auto{__PARA__L1}
    for (j = 0; j < 410; j++) {
      tmp[i] += A[i][j] * x[j];
    }
    
#pragma ACCEL PARALLEL reduction=y FACTOR=auto{__PARA__L2}
    for (j = 0; j < 410; j++) {
      y[j] += A[i][j] * tmp[i];
    }
  }
}
