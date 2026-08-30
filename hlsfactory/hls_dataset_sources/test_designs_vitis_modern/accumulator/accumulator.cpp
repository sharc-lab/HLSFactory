#define N 1024

extern "C" void accumulator(int in[N], int *out) {
#pragma HLS INTERFACE mode=ap_memory port=in
#pragma HLS INTERFACE mode=ap_none port=out

  int sum = 0;
  for (int i = 0; i < N; i++) {
#pragma HLS PIPELINE II=1
    sum += in[i];
  }
  *out = sum;
}
