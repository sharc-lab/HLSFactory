
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
  float [1][64][128],
  float [128],
  float [128][128],
  float [128],
  float [128][128],
  float [128],
  float [128][128],
  float [128],
  float [128][128],
  float [1][64][128]
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

  float v0[1][64][128];
  std::ifstream input_0ifs(prj_path_str + "/data/input_0.bin", std::ios::binary);
  if (input_0ifs.is_open()) {
    input_0ifs.read((char*)v0, 8192 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_0.bin" << std::endl;
  }
  float v1[128];
  std::ifstream weightifs(prj_path_str + "/data/weight.bin", std::ios::binary);
  if (weightifs.is_open()) {
    weightifs.read((char*)v1, 128 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight.bin" << std::endl;
  }
  float v2[128][128];
  std::ifstream weight_1ifs(prj_path_str + "/data/weight_1.bin", std::ios::binary);
  if (weight_1ifs.is_open()) {
    weight_1ifs.read((char*)v2, 16384 * sizeof(float));
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
  float v4[128][128];
  std::ifstream weight_3ifs(prj_path_str + "/data/weight_3.bin", std::ios::binary);
  if (weight_3ifs.is_open()) {
    weight_3ifs.read((char*)v4, 16384 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_3.bin" << std::endl;
  }
  float v5[128];
  std::ifstream weight_4ifs(prj_path_str + "/data/weight_4.bin", std::ios::binary);
  if (weight_4ifs.is_open()) {
    weight_4ifs.read((char*)v5, 128 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_4.bin" << std::endl;
  }
  float v6[128][128];
  std::ifstream weight_5ifs(prj_path_str + "/data/weight_5.bin", std::ios::binary);
  if (weight_5ifs.is_open()) {
    weight_5ifs.read((char*)v6, 16384 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_5.bin" << std::endl;
  }
  float v7[128];
  std::ifstream weight_6ifs(prj_path_str + "/data/weight_6.bin", std::ios::binary);
  if (weight_6ifs.is_open()) {
    weight_6ifs.read((char*)v7, 128 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_6.bin" << std::endl;
  }
  float v8[128][128];
  std::ifstream weight_7ifs(prj_path_str + "/data/weight_7.bin", std::ios::binary);
  if (weight_7ifs.is_open()) {
    weight_7ifs.read((char*)v8, 16384 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_7.bin" << std::endl;
  }
  float v9[1][64][128];
  forward(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9);
  bool v10 = {1};
  float v11[1][64][128];
  std::ifstream output_0ifs(prj_path_str + "/data/output_0.bin", std::ios::binary);
  if (output_0ifs.is_open()) {
    output_0ifs.read((char*)v11, 8192 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/output_0.bin" << std::endl;
  }
  loop0: for (int v12 = 0; v12 < 1; v12++) {
    loop1: for (int v13 = 0; v13 < 64; v13++) {
      loop2: for (int v14 = 0; v14 < 128; v14++) {
        float v15 = v9[v12][v13][v14];
        float v16 = v11[v12][v13][v14];
        float v17 = v15 - v16;
        float v18 = v16 - v15;
        bool v19 = v17 > 0.100000;
        bool v20 = v18 > 0.100000;
        bool v21 = v19 | v20;
        if (v21) {
          v10 = 0;
        }
      }
    }
  }
  bool v22 = v10;
  assert(v22 && "Error!");
  return 0;
}

