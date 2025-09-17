#include "jacobi-2d.h"

int main() {
    int tsteps;
    int n;
    double A[90][90];
    double B[90][90];
    kernel_jacobi_2d(tsteps, n, A, B);
    return 0;
}