
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
  float [1][512][128],
  float [256],
  float [256][128],
  float [128],
  float [128][256],
  float [1][512][128]
);
int32_t main(int argc, char **argv) {
  // get environment variable PRJ_PATH
  char* prj_path_c_str = getenv("PRJ_PATH");
  if (prj_path_c_str == NULL) {
    std::cerr << "Environment variable PRJ_PATH not set" << std::endl;
    return 1;
  }
  std::string prj_path_str(prj_path_c_str);
  std::cout << "PRJ_PATH: " << prj_path_str << std::endl;

  float v0[1][512][128];
  std::ifstream input_0ifs(prj_path_str + "/data/input_0.bin", std::ios::binary);
  if (input_0ifs.is_open()) {
    input_0ifs.read((char*)v0, 65536 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_0.bin" << std::endl;
  }
  float v1[256];
  std::ifstream weightifs(prj_path_str + "/data/weight.bin", std::ios::binary);
  if (weightifs.is_open()) {
    weightifs.read((char*)v1, 256 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight.bin" << std::endl;
  }
  float v2[256][128];
  std::ifstream weight_1ifs(prj_path_str + "/data/weight_1.bin", std::ios::binary);
  if (weight_1ifs.is_open()) {
    weight_1ifs.read((char*)v2, 32768 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_1.bin" << std::endl;
  }
  float v3[128];
  std::ifstream weight_2ifs(prj_path_str + "/data/weight_2.bin", std::ios::binary);
  if (weight_2ifs.is_open()) {
    weight_2ifs.read((char*)v3, 128 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_2.bin" << std::endl;
  }
  float v4[128][256];
  std::ifstream weight_3ifs(prj_path_str + "/data/weight_3.bin", std::ios::binary);
  if (weight_3ifs.is_open()) {
    weight_3ifs.read((char*)v4, 32768 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_3.bin" << std::endl;
  }
  float v5[1][512][128];
  forward(v0, v1, v2, v3, v4, v5);
  bool v6 = {1};
  float v7[1][512][128];
  std::ifstream output_0ifs(prj_path_str + "/data/output_0.bin", std::ios::binary);
  if (output_0ifs.is_open()) {
    output_0ifs.read((char*)v7, 65536 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/output_0.bin" << std::endl;
  }
  loop0: for (int v8 = 0; v8 < 1; v8++) {
    loop1: for (int v9 = 0; v9 < 512; v9++) {
      loop2: for (int v10 = 0; v10 < 128; v10++) {
        float v11 = v5[v8][v9][v10];
        float v12 = v7[v8][v9][v10];
        float v13 = v11 - v12;
        float v14 = v12 - v11;
        bool v15 = v13 > 0.100000;
        bool v16 = v14 > 0.100000;
        bool v17 = v15 | v16;
        if (v17) {
          v6 = 0;
        }
      }
    }
  }
  bool v18 = v6;
  assert(v18 && "Error!");
  return 0;
}

