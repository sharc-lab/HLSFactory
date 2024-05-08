#pragma ACCEL kernel

void kernel_gesummv(int n,double alpha,double beta,double A[90][90],double B[90][90],double tmp[90],double x[90],double y[90])
{
  int i;
  int j;
  
#pragma scop
  
#pragma ACCEL PIPELINE auto{__PIPE__L0}
  
#pragma ACCEL TILE FACTOR=auto{__TILE__L0}
  
#pragma ACCEL PARALLEL FACTOR=auto{__PARA__L0}
  for (i = 0; i < 90; i++) {
    tmp[i] = 0.0;
    y[i] = 0.0;
    
#pragma ACCEL PARALLEL FACTOR=auto{__PARA__L1}
    for (j = 0; j < 90; j++) {
      tmp[i] = A[i][j] * x[j] + tmp[i];
      y[i] = B[i][j] * x[j] + y[i];
    }
    y[i] = alpha * tmp[i] + beta * y[i];
  }
  
#pragma endscop
}
