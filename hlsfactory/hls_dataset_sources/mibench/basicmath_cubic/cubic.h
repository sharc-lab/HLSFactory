#ifndef CUBIC_H
#define CUBIC_H

#define CUBIC_N 64

/*
 * Solves coef[i][0]*x^3 + coef[i][1]*x^2 + coef[i][2]*x + coef[i][3] = 0
 * for each of CUBIC_N equations.  nsol[i] is 1 or 3 and the real roots are
 * stored in x[i][0..nsol[i]-1].
 */
void solve_cubics(const double coef[CUBIC_N][4], int nsol[CUBIC_N],
                  double x[CUBIC_N][3]);

#endif
