#include "gemm-p.h"

int main() {
    int ni;
    int nj;
    int nk;
    double alpha;
    double beta;
    double C[60][70];
    double A[60][80];
    double B[80][70];
    kernel_gemm(ni, nj, nk, alpha, beta, C, A, B);
    return 0;
}