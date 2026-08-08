#include "overlapping.h"

int main() {
    int i;
    static int test_epsilon[N];
    for (i = 0; i < N; i++) {
        test_epsilon[i] = i * 73 % 7 == 0;
    }
    double result;
    Overlapping(&result, test_epsilon);

    printf("result = %.0f\n", result);
    return 0;
}
