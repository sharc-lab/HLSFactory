#include "runs.h"

int main() {
    int i;
    static int epsilon_test[N];
    for (i = 0; i < N; i++) {
        epsilon_test[i] = i * 73 % 7 == 0;
    }
    int result_S, result_V;
    Runs(&result_S, &result_V, epsilon_test);

    printf("result S = %d\n", result_S);
    printf("result V = %d\n", result_V);
    return 0;
}
