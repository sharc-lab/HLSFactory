
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
  float [32][784],
  float [256],
  float [256][784],
  float [128],
  float [128][256],
  float [64],
  float [64][128],
  float [128],
  float [128][64],
  float [256],
  float [256][128],
  float [784],
  float [784][256],
  float [32][784]
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
  float v0[32][784];
  std::ifstream input_0ifs(prj_path_str + "/data/input_0.bin", std::ios::binary);
  if (input_0ifs.is_open()) {
    input_0ifs.read((char*)v0, 25088 * sizeof(float));
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
  float v2[256][784];
  std::ifstream weight_1ifs(prj_path_str + "/data/weight_1.bin", std::ios::binary);
  if (weight_1ifs.is_open()) {
    weight_1ifs.read((char*)v2, 200704 * sizeof(float));
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
  float v5[64];
  std::ifstream weight_4ifs(prj_path_str + "/data/weight_4.bin", std::ios::binary);
  if (weight_4ifs.is_open()) {
    weight_4ifs.read((char*)v5, 64 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_4.bin" << std::endl;
  }
  float v6[64][128];
  std::ifstream weight_5ifs(prj_path_str + "/data/weight_5.bin", std::ios::binary);
  if (weight_5ifs.is_open()) {
    weight_5ifs.read((char*)v6, 8192 * sizeof(float));
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
  float v8[128][64];
  std::ifstream weight_7ifs(prj_path_str + "/data/weight_7.bin", std::ios::binary);
  if (weight_7ifs.is_open()) {
    weight_7ifs.read((char*)v8, 8192 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_7.bin" << std::endl;
  }
  float v9[256];
  std::ifstream weight_8ifs(prj_path_str + "/data/weight_8.bin", std::ios::binary);
  if (weight_8ifs.is_open()) {
    weight_8ifs.read((char*)v9, 256 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_8.bin" << std::endl;
  }
  float v10[256][128];
  std::ifstream weight_9ifs(prj_path_str + "/data/weight_9.bin", std::ios::binary);
  if (weight_9ifs.is_open()) {
    weight_9ifs.read((char*)v10, 32768 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_9.bin" << std::endl;
  }
  float v11[784];
  std::ifstream weight_10ifs(prj_path_str + "/data/weight_10.bin", std::ios::binary);
  if (weight_10ifs.is_open()) {
    weight_10ifs.read((char*)v11, 784 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_10.bin" << std::endl;
  }
  float v12[784][256];
  std::ifstream weight_11ifs(prj_path_str + "/data/weight_11.bin", std::ios::binary);
  if (weight_11ifs.is_open()) {
    weight_11ifs.read((char*)v12, 200704 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/weight_11.bin" << std::endl;
  }
  float v13[32][784];	// L17
  forward(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13);	// L18
  bool v14 = {1};	// L19
  float v15[32][784];
  std::ifstream output_0ifs(prj_path_str + "/data/output_0.bin", std::ios::binary);
  if (output_0ifs.is_open()) {
    output_0ifs.read((char*)v15, 25088 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/output_0.bin" << std::endl;
  }
  loop0: for (int v16 = 0; v16 < 32; v16++) {	// L21
    loop1: for (int v17 = 0; v17 < 784; v17++) {	// L22
      float v18 = v13[v16][v17];	// L23
      float v19 = v15[v16][v17];	// L24
      float v20 = v18 - v19;	// L25
      float v21 = v19 - v18;	// L26
      bool v22 = v20 > 0.100000;	// L28
      bool v23 = v21 > 0.100000;	// L29
      bool v24 = v22 | v23;	// L30
      if (v24) {	// L31
        v14 = 0;	// L34
      }
    }
  }
  bool v25 = v14;	// L39
  assert(v25 && "Error!");	// L40
  return 0;	// L42
}
