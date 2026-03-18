#include "matrix_mul.h"
#include "params.h"
#include <stdio.h>

int main() {
    dtype A[M * M];
    int B[M * M];
    int C[M * M];
    for (int i = 0; i < M * M; i++) {
        A[i].data = 1;
        A[i].last = (i == M * M - 1) ? 1 : 0;
        B[i] = 1;
        C[i] = 0;
    }
    matrix_mul(A, B, C);
    printf("matrix_mul csim done\n");
    return 0;
}
