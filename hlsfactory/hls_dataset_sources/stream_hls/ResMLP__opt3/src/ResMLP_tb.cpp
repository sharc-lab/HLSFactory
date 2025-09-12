
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
  float [8][1024],
  float [512],
  float [512][1024],
  float [512],
  float [512][512],
  float [256],
  float [256][512],
  float [10],
  float [10][256],
  float [8][10]
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
	// L3
  float v0[8][1024];
  std::ifstream input_0ifs(prj_path_str + "/data/input_0.bin", std::ios::binary);
  if (input_0ifs.is_open()) {
    input_0ifs.read((char*)v0, 8192 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_0.bin" << std::endl;
  }
  float v1[512];
  std::ifstream weightifs(prj_path_str + "/data/weight.bin", std::ios::binary);
  if (weightifs.is_open()) {
    weightifs.read((char*)v1, 512 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight.bin" << std::endl;
  }
  float v2[512][1024];
  std::ifstream weight_1ifs(prj_path_str + "/data/weight_1.bin", std::ios::binary);
  if (weight_1ifs.is_open()) {
    weight_1ifs.read((char*)v2, 524288 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_1.bin" << std::endl;
  }
  float v3[512];
  std::ifstream weight_2ifs(prj_path_str + "/data/weight_2.bin", std::ios::binary);
  if (weight_2ifs.is_open()) {
    weight_2ifs.read((char*)v3, 512 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_2.bin" << std::endl;
  }
  float v4[512][512];
  std::ifstream weight_3ifs(prj_path_str + "/data/weight_3.bin", std::ios::binary);
  if (weight_3ifs.is_open()) {
    weight_3ifs.read((char*)v4, 262144 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_3.bin" << std::endl;
  }
  float v5[256];
  std::ifstream weight_4ifs(prj_path_str + "/data/weight_4.bin", std::ios::binary);
  if (weight_4ifs.is_open()) {
    weight_4ifs.read((char*)v5, 256 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_4.bin" << std::endl;
  }
  float v6[256][512];
  std::ifstream weight_5ifs(prj_path_str + "/data/weight_5.bin", std::ios::binary);
  if (weight_5ifs.is_open()) {
    weight_5ifs.read((char*)v6, 131072 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_5.bin" << std::endl;
  }
  float v7[10];
  std::ifstream weight_6ifs(prj_path_str + "/data/weight_6.bin", std::ios::binary);
  if (weight_6ifs.is_open()) {
    weight_6ifs.read((char*)v7, 10 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_6.bin" << std::endl;
  }
  float v8[10][256];
  std::ifstream weight_7ifs(prj_path_str + "/data/weight_7.bin", std::ios::binary);
  if (weight_7ifs.is_open()) {
    weight_7ifs.read((char*)v8, 2560 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_7.bin" << std::endl;
  }
  float v9[8][10];	// L13
  forward(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9);	// L14
  bool v10 = {1};	// L15
  float v11[8][10];
  std::ifstream output_0ifs(prj_path_str + "/data/output_0.bin", std::ios::binary);
  if (output_0ifs.is_open()) {
    output_0ifs.read((char*)v11, 80 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/output_0.bin" << std::endl;
  }
  loop0: for (int v12 = 0; v12 < 8; v12++) {	// L17
    loop1: for (int v13 = 0; v13 < 10; v13++) {	// L18
      float v14 = v9[v12][v13];	// L19
      float v15 = v11[v12][v13];	// L20
      float v16 = v14 - v15;	// L21
      float v17 = v15 - v14;	// L22
      bool v18 = v16 > 0.100000;	// L24
      bool v19 = v17 > 0.100000;	// L25
      bool v20 = v18 | v19;	// L26
      if (v20) {	// L27
        v10 = 0;	// L30
      }
    }
  }
  bool v21 = v10;	// L35
  assert(v21 && "Error!");	// L36
  return 0;	// L38
}
