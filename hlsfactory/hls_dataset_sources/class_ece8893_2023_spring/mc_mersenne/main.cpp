#include "mersenneTwister.hpp"
#include <cstdio>

int main() {
    int seed[1] = {4357};
    double out[NUM];
    randomGen(seed, out);
    std::printf("%g\n", out[0]);
    return 0;
}
