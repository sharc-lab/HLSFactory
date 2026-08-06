#include "./genRandNums.hpp"
#include <ostream>
#include <iostream>
int main(){
    int seed[1];
    seed[0] = 5633;
    double nums[100][100];

    genRandNums(seed, nums);
    for(int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            std::cout << nums[i][j] << " ";
        }
        std::cout << std::endl;
    }
}