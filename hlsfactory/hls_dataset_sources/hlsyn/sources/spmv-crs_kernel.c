#pragma ACCEL kernel

void spmv(double val[1666],int cols[1666],int rowDelimiters[495],double vec[494],double out[494])
{
  int i;
  int j;
  double sum;
  double Si;
  
#pragma ACCEL PIPELINE auto{__PIPE__L0}
  
#pragma ACCEL TILE FACTOR=auto{__TILE__L0}
  
#pragma ACCEL PARALLEL FACTOR=auto{__PARA__L0}
  spmv_1:
  for (i = 0; i < 494; i++) {
    sum = ((double )0);
    Si = ((double )0);
    int tmp_begin = rowDelimiters[i];
    int tmp_end = rowDelimiters[i + 1];
    spmv_2:
    for (j = tmp_begin; j < tmp_end; j++) {
      Si = val[j] * vec[cols[j]];
      sum = sum + Si;
    }
    out[i] = sum;
  }
}
