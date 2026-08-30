#include <cstdio>

#define N 1024

extern "C" void accumulator(int in[N], int *out);

int main() {
  int in[N];
  int expected = 0;

  for (int i = 0; i < N; i++) {
    in[i] = i;
    expected += i;
  }

  int out = 0;
  accumulator(in, &out);

  if (out != expected) {
    printf("MISMATCH: got %d expected %d\n", out, expected);
    return 1;
  }

  printf("PASS\n");
  return 0;
}
