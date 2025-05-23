
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
  float [64][64],
  float [64][64],
  float [64][64],
  float [64][64],
  float [64][64],
  float [64][64],
  float [64][64],
  float [64][64],
  float [64][64]
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

  float v0[64][64];
  std::ifstream input_0ifs(prj_path_str + "/data/input_0.bin", std::ios::binary);
  if (input_0ifs.is_open()) {
    input_0ifs.read((char*)v0, 4096 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_0.bin" << std::endl;
  }
  float v1[64][64];
  std::ifstream input_1ifs(prj_path_str + "/data/input_1.bin", std::ios::binary);
  if (input_1ifs.is_open()) {
    input_1ifs.read((char*)v1, 4096 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_1.bin" << std::endl;
  }
  float v2[64][64];
  std::ifstream input_2ifs(prj_path_str + "/data/input_2.bin", std::ios::binary);
  if (input_2ifs.is_open()) {
    input_2ifs.read((char*)v2, 4096 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_2.bin" << std::endl;
  }
  float v3[64][64];
  std::ifstream input_3ifs(prj_path_str + "/data/input_3.bin", std::ios::binary);
  if (input_3ifs.is_open()) {
    input_3ifs.read((char*)v3, 4096 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_3.bin" << std::endl;
  }
  float v4[64][64];
  std::ifstream input_4ifs(prj_path_str + "/data/input_4.bin", std::ios::binary);
  if (input_4ifs.is_open()) {
    input_4ifs.read((char*)v4, 4096 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_4.bin" << std::endl;
  }
  float v5[64][64];
  std::ifstream input_5ifs(prj_path_str + "/data/input_5.bin", std::ios::binary);
  if (input_5ifs.is_open()) {
    input_5ifs.read((char*)v5, 4096 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_5.bin" << std::endl;
  }
  float v6[64][64];
  std::ifstream input_6ifs(prj_path_str + "/data/input_6.bin", std::ios::binary);
  if (input_6ifs.is_open()) {
    input_6ifs.read((char*)v6, 4096 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_6.bin" << std::endl;
  }
  float v7[64][64];
  std::ifstream input_7ifs(prj_path_str + "/data/input_7.bin", std::ios::binary);
  if (input_7ifs.is_open()) {
    input_7ifs.read((char*)v7, 4096 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_7.bin" << std::endl;
  }
  float v8[64][64];
  forward(v0, v1, v2, v3, v4, v5, v6, v7, v8);
  bool v9 = {1};
  float v10[64][64];
  std::ifstream output_0ifs(prj_path_str + "/data/output_0.bin", std::ios::binary);
  if (output_0ifs.is_open()) {
    output_0ifs.read((char*)v10, 4096 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/output_0.bin" << std::endl;
  }
  loop0: for (int v11 = 0; v11 < 64; v11++) {
    loop1: for (int v12 = 0; v12 < 64; v12++) {
      float v13 = v8[v11][v12];
      float v14 = v10[v11][v12];
      float v15 = v13 - v14;
      float v16 = v14 - v13;
      bool v17 = v15 > 0.100000;
      bool v18 = v16 > 0.100000;
      bool v19 = v17 | v18;
      if (v19) {
        v9 = 0;
      }
    }
  }
  bool v20 = v9;
  assert(v20 && "Error!");
  return 0;
}
