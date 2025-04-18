#include "./optionValueAverage.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>


using namespace std::chrono;

int main() {
    double pth[NUM_PATHS][STEPS];
    double params[4];

    int S0 = params[0] = 86;
    double T = params[1] = 1/12;
    double SIGMA = params[2]  = 0.50;
    double MU  = params[3] = 0.07;

    std::ifstream fin("data1000.csv");
    std::string line;
    std::string substr;


    int i = 0;
    int j = 0;


    while(fin.good()) {
        getline(fin, line);

        std::stringstream ss(line);
        
        try {
            while(ss.good()) {
                getline(ss, substr, ',');
                pth[i][j] = std::stod(substr);               
                i++;
                if(i == NUM_PATHS) {
                    j++;
                    i=0;
                }
            }
        } catch(...) {
            continue;
        }

    }
    
    optionValue(pth, params);


    //std::cout << "SUMMING" << std::endl;
    //Test output
    double sum = 0;
    SUM:
    for(int i = 0; i < NUM_PATHS; i++) {
        sum += (pth[i][STEPS-1])/NUM_PATHS;
    } 

   std::cout << sum << "\n";

   return 0;
}