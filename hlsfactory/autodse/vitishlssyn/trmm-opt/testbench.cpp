#include "trmm-opt.h"

int main() {
    double alpha;
    double A[60][60];
    double B[60][80];
    kernel_trmm(alpha, A, B);
    return 0;
}