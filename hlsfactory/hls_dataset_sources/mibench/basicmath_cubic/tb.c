#include <math.h>
#include <stdio.h>
#include "cubic.h"

int main(void)
{
    static double coef[CUBIC_N][4];
    static double x[CUBIC_N][3];
    static int nsol[CUBIC_N];
    int errors = 0, n = 0;

    /* the four fixed equations of basicmath_small.c */
    const double fixed[4][4] = {
        {1.0, -10.5, 32.0, -30.0}, /* 3 roots: 2, 6 and 2.5 per source comment (see check) */
        {1.0, -4.5, 17.0, -30.0},  /* 1 root: 2.5 */
        {1.0, -3.5, 22.0, -31.0},
        {1.0, -13.7, 1.0, -35.0},
    };
    for (; n < 4; n++)
        for (int k = 0; k < 4; k++)
            coef[n][k] = fixed[n][k];

    /* a slice of the random-equation sweep: a=1..9, b=10..1, c=5..15, d=-1..-10 */
    for (double a = 1; a < 10 && n < CUBIC_N; a += 4)
        for (double b = 10; b > 0 && n < CUBIC_N; b -= 3)
            for (double c = 5; c < 15 && n < CUBIC_N; c += 2.5)
                for (double d = -1; d > -11 && n < CUBIC_N; d -= 3) {
                    coef[n][0] = a;
                    coef[n][1] = b;
                    coef[n][2] = c;
                    coef[n][3] = d;
                    n++;
                }
    for (; n < CUBIC_N; n++) {
        coef[n][0] = 1.0;
        coef[n][1] = 0.0;
        coef[n][2] = -(double)n;
        coef[n][3] = 1.0;
    }

    solve_cubics(coef, nsol, x);

    /* every reported root must satisfy the polynomial */
    for (int i = 0; i < CUBIC_N; i++) {
        if (nsol[i] != 1 && nsol[i] != 3) {
            printf("eq %d: bad solution count %d\n", i, nsol[i]);
            errors++;
            continue;
        }
        for (int r = 0; r < nsol[i]; r++) {
            double v = x[i][r];
            double p = ((coef[i][0] * v + coef[i][1]) * v + coef[i][2]) * v + coef[i][3];
            double scale = fabs(coef[i][0] * v * v * v) + fabs(coef[i][1] * v * v) +
                           fabs(coef[i][2] * v) + fabs(coef[i][3]);
            if (!(fabs(p) <= 1e-6 * scale)) {
                printf("eq %d root %d = %f: residual %g\n", i, r, v, p);
                errors++;
            }
        }
    }
    /* x^3 - 4.5x^2 + 17x - 30 has a single real root, 2.5 */
    if (nsol[1] != 1 || fabs(x[1][0] - 2.5) > 1e-9) {
        printf("eq 1: expected single root 2.5, got %d roots, first %f\n", nsol[1], x[1][0]);
        errors++;
    }

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS\n");
    return 0;
}
