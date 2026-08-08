#include "monobit.h"

int main() {
    int i;
    int epsilon_test[N];
    for (i = 0; i < N; i++) {
        epsilon_test[i] = i * 73 % 7 == 0;
    }
    int result;
    Frequency(&result, epsilon_test);

    printf("result = %d\n", result);
    return 0;
}
