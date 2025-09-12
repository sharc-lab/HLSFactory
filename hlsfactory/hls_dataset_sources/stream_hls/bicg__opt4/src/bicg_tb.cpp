
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
  float [410][390],
  float [410][390],
  float [410],
  float [390],
  float [390],
  float [410]
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

  float v0[410][390];
  std::ifstream input_0ifs(prj_path_str + "/data/input_0.bin", std::ios::binary);
  if (input_0ifs.is_open()) {
    input_0ifs.read((char*)v0, 159900 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_0.bin" << std::endl;
  }
  float v1[410][390];
  std::ifstream input_1ifs(prj_path_str + "/data/input_1.bin", std::ios::binary);
  if (input_1ifs.is_open()) {
    input_1ifs.read((char*)v1, 159900 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_1.bin" << std::endl;
  }
  float v2[410];
  std::ifstream input_2ifs(prj_path_str + "/data/input_2.bin", std::ios::binary);
  if (input_2ifs.is_open()) {
    input_2ifs.read((char*)v2, 410 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_2.bin" << std::endl;
  }
  float v3[390];
  std::ifstream input_3ifs(prj_path_str + "/data/input_3.bin", std::ios::binary);
  if (input_3ifs.is_open()) {
    input_3ifs.read((char*)v3, 390 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_3.bin" << std::endl;
  }
  float v4[390];
  float v5[410];
  forward(v0, v1, v2, v3, v4, v5);
  bool v6 = {1};
  float v7[390];
  std::ifstream output_0ifs(prj_path_str + "/data/output_0.bin", std::ios::binary);
  if (output_0ifs.is_open()) {
    output_0ifs.read((char*)v7, 390 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/output_0.bin" << std::endl;
  }
  loop0: for (int v8 = 0; v8 < 390; v8++) {
    float v9 = v4[v8];
    float v10 = v7[v8];
    float v11 = v9 - v10;
    float v12 = v10 - v9;
    bool v13 = v11 > 0.100000;
    bool v14 = v12 > 0.100000;
    bool v15 = v13 | v14;
    if (v15) {
      v6 = 0;
    }
  }
  float v16[410];
  std::ifstream output_1ifs(prj_path_str + "/data/output_1.bin", std::ios::binary);
  if (output_1ifs.is_open()) {
    output_1ifs.read((char*)v16, 410 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/output_1.bin" << std::endl;
  }
  loop1: for (int v17 = 0; v17 < 410; v17++) {
    float v18 = v5[v17];
    float v19 = v16[v17];
    float v20 = v18 - v19;
    float v21 = v19 - v18;
    bool v22 = v20 > 0.100000;
    bool v23 = v21 > 0.100000;
    bool v24 = v22 | v23;
    if (v24) {
      v6 = 0;
    }
  }
  bool v25 = v6;
  assert(v25 && "Error!");
  return 0;
}
