#define N 32

void kernel_plain(int A[N][N], int B[N][N], int C[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            int acc = 0;
            for (int k = 0; k < N; k++) {
                acc += A[i][k] * B[k][j];
            }
            C[i][j] = acc;
        }
    }
}
