
//===------------------------------------------------------------*- C++ -*-===//
//
// Automatically generated file for host
//
//===----------------------------------------------------------------------===//
// XRT includes
// #include "xrt/xrt_bo.h"
// #include <experimental/xrt_xclbin.h>
// #include "xrt/xrt_device.h"
// #include "xrt/xrt_kernel.h"

// standard C/C++ headers
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <time.h>

#include <math.h>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <cstring>

#include <hls_half.h>

using namespace std;

void forward(
  float [1][16][56][56],
  float [16][16][3][3],
  float [16][16][3][3],
  float [1][16][56][56]
);
int32_t main(int argc, char **argv) {
  // get environment variable PRJ_PATH
//   char* prj_path_c_str = getenv("PRJ_PATH");
//   if (prj_path_c_str == NULL) {
//     std::cerr << "Environment variable PRJ_PATH not set" << std::endl;
//     return 1;
//   }
//   std::string prj_path_str(prj_path_c_str);
//   std::cout << "PRJ_PATH: " << prj_path_str << std::endl;
  std::string prj_path_str = ".";

  float v0[1][16][56][56];
  std::ifstream input_0ifs(prj_path_str + "/data/input_0.bin", std::ios::binary);
  if (input_0ifs.is_open()) {
    input_0ifs.read((char*)v0, 50176 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_0.bin" << std::endl;
  }
  float v1[16][16][3][3];
  std::ifstream weightifs(prj_path_str + "/data/weight.bin", std::ios::binary);
  if (weightifs.is_open()) {
    weightifs.read((char*)v1, 2304 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight.bin" << std::endl;
  }
  float v2[16][16][3][3];
  std::ifstream weight_1ifs(prj_path_str + "/data/weight_1.bin", std::ios::binary);
  if (weight_1ifs.is_open()) {
    weight_1ifs.read((char*)v2, 2304 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_1.bin" << std::endl;
  }
  float v3[1][16][56][56];
  forward(v0, v1, v2, v3);
  bool v4 = {1};
  float v5[1][16][56][56];
  std::ifstream output_0ifs(prj_path_str + "/data/output_0.bin", std::ios::binary);
  if (output_0ifs.is_open()) {
    output_0ifs.read((char*)v5, 50176 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/output_0.bin" << std::endl;
  }
  loop0: for (int v6 = 0; v6 < 1; v6++) {
    loop1: for (int v7 = 0; v7 < 16; v7++) {
      loop2: for (int v8 = 0; v8 < 56; v8++) {
        loop3: for (int v9 = 0; v9 < 56; v9++) {
          float v10 = v3[v6][v7][v8][v9];
          float v11 = v5[v6][v7][v8][v9];
          float v12 = v10 - v11;
          float v13 = v11 - v10;
          bool v14 = v12 > 0.100000;
          bool v15 = v13 > 0.100000;
          bool v16 = v14 | v15;
          if (v16) {
            v4 = 0;
          }
        }
      }
    }
  }
  bool v17 = v4;
  assert(v17 && "Error!");
  return 0;
}
