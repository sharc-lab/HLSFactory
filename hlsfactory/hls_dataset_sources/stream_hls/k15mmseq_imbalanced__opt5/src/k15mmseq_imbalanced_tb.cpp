
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
  float [16][32],
  float [32][128],
  float [128][64],
  float [64][128],
  float [128][64],
  float [64][32],
  float [32][16],
  float [16][16],
  float [16][32],
  float [32][128],
  float [128][64],
  float [64][128],
  float [128][64],
  float [64][32],
  float [32][16],
  float [16][16],
  float [16][16]
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

  float v0[16][32];
  std::ifstream input_0ifs(prj_path_str + "/data/input_0.bin", std::ios::binary);
  if (input_0ifs.is_open()) {
    input_0ifs.read((char*)v0, 512 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_0.bin" << std::endl;
  }
  float v1[32][128];
  std::ifstream input_1ifs(prj_path_str + "/data/input_1.bin", std::ios::binary);
  if (input_1ifs.is_open()) {
    input_1ifs.read((char*)v1, 4096 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_1.bin" << std::endl;
  }
  float v2[128][64];
  std::ifstream input_2ifs(prj_path_str + "/data/input_2.bin", std::ios::binary);
  if (input_2ifs.is_open()) {
    input_2ifs.read((char*)v2, 8192 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_2.bin" << std::endl;
  }
  float v3[64][128];
  std::ifstream input_3ifs(prj_path_str + "/data/input_3.bin", std::ios::binary);
  if (input_3ifs.is_open()) {
    input_3ifs.read((char*)v3, 8192 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_3.bin" << std::endl;
  }
  float v4[128][64];
  std::ifstream input_4ifs(prj_path_str + "/data/input_4.bin", std::ios::binary);
  if (input_4ifs.is_open()) {
    input_4ifs.read((char*)v4, 8192 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_4.bin" << std::endl;
  }
  float v5[64][32];
  std::ifstream input_5ifs(prj_path_str + "/data/input_5.bin", std::ios::binary);
  if (input_5ifs.is_open()) {
    input_5ifs.read((char*)v5, 2048 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_5.bin" << std::endl;
  }
  float v6[32][16];
  std::ifstream input_6ifs(prj_path_str + "/data/input_6.bin", std::ios::binary);
  if (input_6ifs.is_open()) {
    input_6ifs.read((char*)v6, 512 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_6.bin" << std::endl;
  }
  float v7[16][16];
  std::ifstream input_7ifs(prj_path_str + "/data/input_7.bin", std::ios::binary);
  if (input_7ifs.is_open()) {
    input_7ifs.read((char*)v7, 256 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_7.bin" << std::endl;
  }
  float v8[16][32];
  std::ifstream input_8ifs(prj_path_str + "/data/input_8.bin", std::ios::binary);
  if (input_8ifs.is_open()) {
    input_8ifs.read((char*)v8, 512 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_8.bin" << std::endl;
  }
  float v9[32][128];
  std::ifstream input_9ifs(prj_path_str + "/data/input_9.bin", std::ios::binary);
  if (input_9ifs.is_open()) {
    input_9ifs.read((char*)v9, 4096 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_9.bin" << std::endl;
  }
  float v10[128][64];
  std::ifstream input_10ifs(prj_path_str + "/data/input_10.bin", std::ios::binary);
  if (input_10ifs.is_open()) {
    input_10ifs.read((char*)v10, 8192 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_10.bin" << std::endl;
  }
  float v11[64][128];
  std::ifstream input_11ifs(prj_path_str + "/data/input_11.bin", std::ios::binary);
  if (input_11ifs.is_open()) {
    input_11ifs.read((char*)v11, 8192 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_11.bin" << std::endl;
  }
  float v12[128][64];
  std::ifstream input_12ifs(prj_path_str + "/data/input_12.bin", std::ios::binary);
  if (input_12ifs.is_open()) {
    input_12ifs.read((char*)v12, 8192 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_12.bin" << std::endl;
  }
  float v13[64][32];
  std::ifstream input_13ifs(prj_path_str + "/data/input_13.bin", std::ios::binary);
  if (input_13ifs.is_open()) {
    input_13ifs.read((char*)v13, 2048 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_13.bin" << std::endl;
  }
  float v14[32][16];
  std::ifstream input_14ifs(prj_path_str + "/data/input_14.bin", std::ios::binary);
  if (input_14ifs.is_open()) {
    input_14ifs.read((char*)v14, 512 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_14.bin" << std::endl;
  }
  float v15[16][16];
  std::ifstream input_15ifs(prj_path_str + "/data/input_15.bin", std::ios::binary);
  if (input_15ifs.is_open()) {
    input_15ifs.read((char*)v15, 256 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/input_15.bin" << std::endl;
  }
  float v16[16][16];
  forward(v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16);
  bool v17 = {1};
  float v18[16][16];
  std::ifstream output_0ifs(prj_path_str + "/data/output_0.bin", std::ios::binary);
  if (output_0ifs.is_open()) {
    output_0ifs.read((char*)v18, 256 * sizeof(float));
  } else {
    std::cerr << "cannot open file /data/output_0.bin" << std::endl;
  }
  loop0: for (int v19 = 0; v19 < 16; v19++) {
    loop1: for (int v20 = 0; v20 < 16; v20++) {
      float v21 = v16[v19][v20];
      float v22 = v18[v19][v20];
      float v23 = v21 - v22;
      float v24 = v22 - v21;
      bool v25 = v23 > 0.100000;
      bool v26 = v24 > 0.100000;
      bool v27 = v25 | v26;
      if (v27) {
        v17 = 0;
      }
    }
  }
  bool v28 = v17;
  assert(v28 && "Error!");
  return 0;
}

