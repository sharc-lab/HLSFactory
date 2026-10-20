/* +++Date last modified: 05-Jul-1997 */
/*
 * MiBench automotive/basicmath (cubic.c): solve a cubic polynomial.
 * public domain by Ross Cottrell (SNIPPETS).
 *
 * HLS adaptation: 'long double' temporaries became 'double', and the top
 * function solves an array of equations.
 */
#include <math.h>
#include "cubic.h"

#define PI (4 * atan(1))

static void SolveCubic(double a, double b, double c, double d,
                       int *solutions, double *x)
{
    double a1 = b / a, a2 = c / a, a3 = d / a;
    double Q = (a1 * a1 - 3.0 * a2) / 9.0;
    double R = (2.0 * a1 * a1 * a1 - 9.0 * a1 * a2 + 27.0 * a3) / 54.0;
    double R2_Q3 = R * R - Q * Q * Q;
    double theta;

    if (R2_Q3 <= 0) {
        *solutions = 3;
        theta = acos(R / sqrt(Q * Q * Q));
        x[0] = -2.0 * sqrt(Q) * cos(theta / 3.0) - a1 / 3.0;
        x[1] = -2.0 * sqrt(Q) * cos((theta + 2.0 * PI) / 3.0) - a1 / 3.0;
        x[2] = -2.0 * sqrt(Q) * cos((theta + 4.0 * PI) / 3.0) - a1 / 3.0;
    } else {
        *solutions = 1;
        x[0] = pow(sqrt(R2_Q3) + fabs(R), 1 / 3.0);
        x[0] += Q / x[0];
        x[0] *= (R < 0.0) ? 1 : -1;
        x[0] -= a1 / 3.0;
    }
}

void solve_cubics(const double coef[CUBIC_N][4], int nsol[CUBIC_N],
                  double x[CUBIC_N][3])
{
    for (int i = 0; i < CUBIC_N; i++) {
        double r[3] = {0.0, 0.0, 0.0};
        int n = 0;
        SolveCubic(coef[i][0], coef[i][1], coef[i][2], coef[i][3], &n, r);
        nsol[i] = n;
        x[i][0] = r[0];
        x[i][1] = r[1];
        x[i][2] = r[2];
    }
}
