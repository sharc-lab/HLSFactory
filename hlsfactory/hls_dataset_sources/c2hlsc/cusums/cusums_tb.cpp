#include "cusums.h"

int main() {
    int i;
    static int epsilon_test[N];
    for (i = 0; i < N; i++) {
        epsilon_test[i] = i * 73 % 7 == 0;
    }
    int res_sup;
    int res_inf;
    CumulativeSums(&res_sup, &res_inf, epsilon_test);

    printf("sup = %d - inf = %d\n", res_sup, res_inf);
    return 0;
}
