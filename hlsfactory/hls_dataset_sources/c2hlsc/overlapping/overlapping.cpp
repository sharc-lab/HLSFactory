
#include "overlapping.h"

void Overlapping(double *result, int epsilon[N]) {
    int i, j, k, match;
    double W_obs, chi2, eta, sum;
    int K = 5;
    unsigned int nu[6] = {0, 0, 0, 0, 0, 0};
    double pi[6] = {2.746566, 5.386218, 7.17457, 9.94322, 14.198031, 7.1497515};
    char sequence[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

    for (i = 0; i < 1024; i++) {
        W_obs = 0;
        for (j = 0; j < 1024; j++) {
            match = 1;
            for (k = 0; k < 9; k++) {
                if (sequence[k] != epsilon[i * 1032 + j + k])
                    match = 0;
            }
            if (match == 1)
                W_obs++;
        }
        if (W_obs <= 4)
            nu[(int)W_obs]++;
        else
            nu[K]++;
    }
    chi2 = 0.0; /* Compute Chi Square */
    for (i = 0; i < 6; i++) {
        chi2 += nu[i] * nu[i] * pi[i];
    }
    *result = chi2;
}
