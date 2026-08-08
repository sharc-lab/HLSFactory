#include "block.h"

int main() {
    int i;
    int epsilon_test[N * M];
    for (i = 0; i < N * M; i++) {
        epsilon_test[i] = i * 73 % 7 == 0;
    }
    double result;
    BlockFrequency(&result, epsilon_test);

    printf("result = %f\n", result);
    return 0;
}
