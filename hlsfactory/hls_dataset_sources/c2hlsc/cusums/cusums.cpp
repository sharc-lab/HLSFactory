#include "cusums.h"

void CumulativeSums(int *res_sup, int *res_inf, int epsilon[N]) {
    int S, sup, inf, k;
    S = 0;
    sup = 0;
    inf = 0;
    for (k = 0; k < N; k++) {
        epsilon[k] ? S++ : S--;
        if (S > sup)
            sup++;
        if (S < inf)
            inf--;
    }
    *res_sup = sup;
    *res_inf = inf;
}
