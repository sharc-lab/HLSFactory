
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
  float [180][200],
  float [200][190],
  float [190][220],
  float [220][210],
  float [180][210]
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

  float v0[180][200];
  std::ifstream input_0ifs(prj_path_str + "/data/input_0.bin", std::ios::binary);
  if (input_0ifs.is_open()) {
    input_0ifs.read((char*)v0, 36000 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_0.bin" << std::endl;
  }
  float v1[200][190];
  std::ifstream input_1ifs(prj_path_str + "/data/input_1.bin", std::ios::binary);
  if (input_1ifs.is_open()) {
    input_1ifs.read((char*)v1, 38000 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_1.bin" << std::endl;
  }
  float v2[190][220];
  std::ifstream input_2ifs(prj_path_str + "/data/input_2.bin", std::ios::binary);
  if (input_2ifs.is_open()) {
    input_2ifs.read((char*)v2, 41800 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_2.bin" << std::endl;
  }
  float v3[220][210];
  std::ifstream input_3ifs(prj_path_str + "/data/input_3.bin", std::ios::binary);
  if (input_3ifs.is_open()) {
    input_3ifs.read((char*)v3, 46200 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_3.bin" << std::endl;
  }
  float v4[180][210];
  forward(v0, v1, v2, v3, v4);
  bool v5 = {1};
  float v6[180][210];
  std::ifstream output_0ifs(prj_path_str + "/data/output_0.bin", std::ios::binary);
  if (output_0ifs.is_open()) {
    output_0ifs.read((char*)v6, 37800 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/output_0.bin" << std::endl;
  }
  loop0: for (int v7 = 0; v7 < 180; v7++) {
    loop1: for (int v8 = 0; v8 < 210; v8++) {
      float v9 = v4[v7][v8];
      float v10 = v6[v7][v8];
      float v11 = v9 - v10;
      float v12 = v10 - v9;
      bool v13 = v11 > 0.100000;
      bool v14 = v12 > 0.100000;
      bool v15 = v13 | v14;
      if (v15) {
        v5 = 0;
      }
    }
  }
  bool v16 = v5;
  assert(v16 && "Error!");
  return 0;
}

