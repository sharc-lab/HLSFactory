#include "block.h"

void BlockFrequency(double *result, int epsilon[M * N]) {
    int i, j, blockSum;
    double sum, pi, v, chi_squared;

    sum = 0.0;

    for (i = 0; i < N; i++) {
        blockSum = 0;
        for (j = 0; j < M; j++)
            blockSum += epsilon[j + i * M];
        pi = (double)blockSum / (double)M;
        v = pi - 0.5;
        sum += v * v;
    }

    *result = sum;
}
