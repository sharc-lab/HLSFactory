//Testbench to verify the functionality
#include "dcl.h"

//Matrix Multiplication function definition
void matrix_mul(int MatA_DRAM[M][N], int MatB_DRAM[N][K], int MatC_DRAM[M][K]);

//Main function
int main()
{
    int MatA_tb[M][N];
    int MatB_tb[N][K];
    int MatC_tb[M][K];
    int MatC_expected[M][K];

    //Initialize Matrix values
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            MatA_tb[i][j] = 1;
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < K; j++) {
            MatB_tb[i][j] = 2;
        }
    }
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
            MatC_tb[i][j] = 0;
            MatC_expected[i][j] = 0;
        }
    }
    matrix_mul(MatA_tb, MatB_tb, MatC_tb);

    // Expected value for MatC_tb
    // To make sure your optimizations in matrix_mul is not changing the functionality
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
            for (int p = 0; p < N; p++) {
                MatC_expected[i][j] += MatA_tb[i][p] * MatB_tb[p][j];
            }
        }
    }

    // Verify if the output of matrix_mul is correct
    int passed = 1;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < K; j++) {
            if (MatC_tb[i][j] != MatC_expected[i][j]) {
                printf("Mismatch at MatC[%d][%d]: expected %d, got %d\n", i, j, MatC_expected[i][j], MatC_tb[i][j]);
                passed = 0;
            }
        }
    }
    if (passed) {
        printf("Your test passed!\n");
    }
    return 0;
}