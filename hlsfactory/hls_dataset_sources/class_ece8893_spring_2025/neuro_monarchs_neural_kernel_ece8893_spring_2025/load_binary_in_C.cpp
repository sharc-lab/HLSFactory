// #include "dcl.h"
// #include <iostream>
// #include <fstream>
// #include <cstdlib>

// float**** loadBinary(const char* filename, int C1, int C2, int C3, int C4) {
//     // Allocate memory for a 4D array of floats
//     float**** array = (float****)malloc(C1 * sizeof(float***));
//     if (!array) {
//         std::cerr << "Error: Allocation failed for dimension C1." << std::endl;
//         return nullptr;
//     }
    
//     for (int i = 0; i < C1; ++i) {
//         array[i] = (float***)malloc(C2 * sizeof(float**));
//         if (!array[i]) {
//             std::cerr << "Error: Allocation failed for dimension C2 at index " << i << std::endl;
//             return nullptr;
//         }
//         for (int j = 0; j < C2; ++j) {
//             array[i][j] = (float**)malloc(C3 * sizeof(float*));
//             if (!array[i][j]) {
//                 std::cerr << "Error: Allocation failed for dimension C3 at index " << i << ", " << j << std::endl;
//                 return nullptr;
//             }
//             for (int k = 0; k < C3; ++k) {
//                 array[i][j][k] = (float*)malloc(C4 * sizeof(float));
//                 if (!array[i][j][k]) {
//                     std::cerr << "Error: Allocation failed for dimension C4 at index " << i << ", " << j << ", " << k << std::endl;
//                     return nullptr;
//                 }
//             }
//         }
//     }
    
//     // Open the binary file for input
//     std::ifstream ifs(filename, std::ios::in | std::ios::binary);
//     if (!ifs.is_open()) {
//         std::cerr << "Error: Failed to open file " << filename << std::endl;
//         return nullptr;
//     }

//     // Read each innermost array separately because memory is not contiguous
//     for (int i = 0; i < C1; ++i) {
//         for (int j = 0; j < C2; ++j) {
//             for (int k = 0; k < C3; ++k) {
//                 ifs.read(reinterpret_cast<char*>(array[i][j][k]), C4 * sizeof(float));
//                 if (!ifs) {
//                     std::cerr << "Error: Failed to read data at position (" 
//                               << i << ", " << j << ", " << k << ")." << std::endl;
//                     return nullptr;
//                 }
//             }
//         }
//     }
    
//     ifs.close();
//     return array;
// }
