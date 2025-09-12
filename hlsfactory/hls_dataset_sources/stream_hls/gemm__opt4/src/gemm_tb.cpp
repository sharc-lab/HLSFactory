
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
  float [200][240],
  float [240][220],
  float [200][220],
  float [200][220]
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

  float v0[200][240];
  std::ifstream input_0ifs(prj_path_str + "/data/input_0.bin", std::ios::binary);
  if (input_0ifs.is_open()) {
    input_0ifs.read((char*)v0, 48000 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_0.bin" << std::endl;
  }
  float v1[240][220];
  std::ifstream input_1ifs(prj_path_str + "/data/input_1.bin", std::ios::binary);
  if (input_1ifs.is_open()) {
    input_1ifs.read((char*)v1, 52800 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_1.bin" << std::endl;
  }
  float v2[200][220];
  std::ifstream input_2ifs(prj_path_str + "/data/input_2.bin", std::ios::binary);
  if (input_2ifs.is_open()) {
    input_2ifs.read((char*)v2, 44000 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_2.bin" << std::endl;
  }
  float v3[200][220];
  forward(v0, v1, v2, v3);
  bool v4 = {1};
  float v5[200][220];
  std::ifstream output_0ifs(prj_path_str + "/data/output_0.bin", std::ios::binary);
  if (output_0ifs.is_open()) {
    output_0ifs.read((char*)v5, 44000 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/output_0.bin" << std::endl;
  }
  loop0: for (int v6 = 0; v6 < 200; v6++) {
    loop1: for (int v7 = 0; v7 < 220; v7++) {
      float v8 = v3[v6][v7];
      float v9 = v5[v6][v7];
      float v10 = v8 - v9;
      float v11 = v9 - v8;
      bool v12 = v10 > 0.100000;
      bool v13 = v11 > 0.100000;
      bool v14 = v12 | v13;
      if (v14) {
        v4 = 0;
      }
    }
  }
  bool v15 = v4;
  assert(v15 && "Error!");
  return 0;
}
