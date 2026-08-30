#define N 1024

extern "C" void vector_add(int a[N], int b[N], int c[N]) {
#pragma HLS INTERFACE mode=ap_memory port=a
#pragma HLS INTERFACE mode=ap_memory port=b
#pragma HLS INTERFACE mode=ap_memory port=c
  for (int i = 0; i < N; i++) {
    c[i] = a[i] + b[i];
  }
}
