#include <cstdio>

#define N 1024

extern "C" void vector_add(int a[N], int b[N], int c[N]);

int main() {
  int a[N];
  int b[N];
  int c[N];

  for (int i = 0; i < N; i++) {
    a[i] = i;
    b[i] = 2 * i;
  }

  vector_add(a, b, c);

  for (int i = 0; i < N; i++) {
    int expected = a[i] + b[i];
    if (c[i] != expected) {
      printf("MISMATCH at %d: got %d expected %d\n", i, c[i], expected);
      return 1;
    }
  }

  printf("PASS\n");
  return 0;
}
