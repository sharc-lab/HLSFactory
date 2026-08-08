#include "runs.h"

void Runs(int *res_S, int *res_V, int epsilon[N]) {
    int S, V, k;

    S = 0;
    for (k = 0; k < 65535; k++)
        if (epsilon[k]) // count ones
            S++;
    V = 1;
    for (k = 1; k < 65535; k++)
        if (epsilon[k] !=
            epsilon[k - 1]) // count changes from 0 to 1 and 1 to 0
            V++;

    // return results through parameter reference
    *res_S = S;
    *res_V = V;
}
