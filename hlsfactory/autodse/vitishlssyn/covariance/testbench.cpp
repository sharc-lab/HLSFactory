#include "covariance.h"

int main() {
    int m;
    int n;
    double float_n;
    double data[100][80];
    double cov[80][80];
    double mean[80];
    kernel_covariance(m, n, float_n, data, cov, mean);
    return 0;
}