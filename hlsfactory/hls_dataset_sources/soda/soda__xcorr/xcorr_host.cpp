#line 20 "soda_designs_processed/soda__xcorr/xcorr_host.cpp"
#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>


#include <ap_int.h>

using std::clog;
using std::endl;
using std::regex;
using std::regex_match;
using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::vector;


extern "C" {
void xcorr_kernel(
  ap_uint<64>* bank_1_tmp3,
  ap_uint<64>* bank_0_input,
  uint64_t coalesced_data_num);
}

namespace soda {
namespace app {

constexpr int kStencilDim0 = 19;
constexpr int kStencilDim1 = 19;
constexpr int kStencilDistance = 8658;
constexpr int kWidth_input = 16;
constexpr int kWidth_tmp3 = 16;


using Type_input = int16_t;
using Type_tmp3 = int16_t;

int xcorr(
  const Type_input* var_input_ptr,
  const int32_t var_input_extent[2],
  const int32_t var_input_stride[2],
  const int32_t var_input_min[2],
  Type_tmp3* var_tmp3_ptr,
  const int32_t var_tmp3_extent[2],
  const int32_t var_tmp3_stride[2],
  const int32_t var_tmp3_min[2],
  // const char* bitstream,
  const int burst_width = 64,
  const int tile_size_0 = 480,
  const int unroll_factor = 4)
{


  int bank_count_input = 1;
  int bank_count_tmp3 = 1;
#line 105 "soda_designs_processed/soda__xcorr/xcorr_host.cpp"
  auto round_up = [](int64_t a, int64_t b) -> int64_t { return ((a - 1) / b + 1) * b; };


  const int elem_count_per_cycle_input = burst_width / kWidth_input * bank_count_input;
  const int elem_count_per_cycle_tmp3 = burst_width / kWidth_tmp3 * bank_count_tmp3;
  int32_t tile_count_dim_0 = (var_input_extent[0] - kStencilDim0 + 1 - 1) / (tile_size_0 - kStencilDim0 + 1) + 1;
  int64_t tile_count = tile_count_dim_0;


  int64_t elem_count_per_tile = tile_size_0 * var_input_extent[1];
  int64_t cycle_count_per_tile = (elem_count_per_tile - 1) / elem_count_per_cycle_input + 1;
  int64_t elem_count_aligned_per_tile_i = cycle_count_per_tile * elem_count_per_cycle_input;
  int64_t elem_count_aligned_per_tile_o = cycle_count_per_tile * elem_count_per_cycle_tmp3;


  int64_t buf_size_input = (tile_count * elem_count_aligned_per_tile_i + round_up(kStencilDistance, elem_count_per_cycle_input)) / bank_count_input * sizeof(Type_input);
  int64_t buf_size_tmp3 = (tile_count * elem_count_aligned_per_tile_o + round_up(kStencilDistance, elem_count_per_cycle_tmp3)) / bank_count_tmp3 * sizeof(Type_tmp3);


  vector<unique_ptr<Type_input, decltype(&free)>> buf_input;
  buf_input.reserve(bank_count_input);
  for (int bank = 0; bank < bank_count_input; ++bank) {
    buf_input.emplace_back(static_cast<Type_input*>(aligned_alloc(4096, round_up(buf_size_input, 4096))), &free);
  }
  vector<unique_ptr<Type_tmp3, decltype(&free)>> buf_tmp3;
  buf_tmp3.reserve(bank_count_tmp3);
  for (int bank = 0; bank < bank_count_tmp3; ++bank) {
    buf_tmp3.emplace_back(static_cast<Type_tmp3*>(aligned_alloc(4096, round_up(buf_size_tmp3, 4096))), &free);
  }


  for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
  {
    int32_t actual_tile_size_dim_0 = (tile_index_dim_0==tile_count_dim_0-1) ? var_input_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

    for(int32_t j = 0; j < var_input_extent[1]; ++j)
    {
      for(int32_t i = 0; i < actual_tile_size_dim_0; ++i)
      {



        int32_t burst_index = (i + j * tile_size_0) / elem_count_per_cycle_input;
        int32_t burst_residue = (i + j * tile_size_0) % elem_count_per_cycle_input;
        int32_t p = tile_index_dim_0 * (tile_size_0 - kStencilDim0) + i;
        int32_t q = j;
        int64_t tiled_offset = (tile_index_dim_0) * elem_count_aligned_per_tile_i + burst_index * elem_count_per_cycle_input + burst_residue;
        int64_t original_offset = p * var_input_stride[0] + q * var_input_stride[1];
        buf_input[tiled_offset % bank_count_input].get()[tiled_offset / bank_count_input] = var_input_ptr[std::max(int64_t(0), original_offset - 0)];
      }
    }
  }

  clog << "INFO: tile_count[0] = " << tile_count_dim_0 << ", tile_size[0] = " << tile_size_0 << endl;
  clog << "INFO: input.extent[0] = " << var_input_extent[0] << ", input.extent[1] = " << var_input_extent[1] << "" << endl;
  clog << "INFO: input.stride[0] = " << var_input_stride[0] << ", input.stride[1] = " << var_input_stride[1] << "" << endl;
  clog << "INFO: input.min[0] = " << var_input_min[0] << ", input.min[1] = " << var_input_min[1] << "" << endl;
  clog << "INFO: tmp3.extent[0] = " << var_tmp3_extent[0] << ", tmp3.extent[1] = " << var_tmp3_extent[1] << "" << endl;
  clog << "INFO: tmp3.stride[0] = " << var_tmp3_stride[0] << ", tmp3.stride[1] = " << var_tmp3_stride[1] << "" << endl;
  clog << "INFO: tmp3.min[0] = " << var_tmp3_min[0] << ", tmp3.min[1] = " << var_tmp3_min[1] << "" << endl;

  int64_t tile_data_count = ((int64_t(var_input_extent[1]) * tile_size_0 - 1) / elem_count_per_cycle_input + 1) * elem_count_per_cycle_input / unroll_factor;
  int64_t cycle_count = ((int64_t(var_input_extent[1]) * tile_size_0 * tile_count_dim_0 + kStencilDistance - 1) / elem_count_per_cycle_input + 1);
  clog << "INFO: tile_data_count = " << tile_data_count << ", cycle_count = " << cycle_count << endl;


  ::xcorr_kernel(
    (ap_uint<64>*)buf_tmp3[0].get(),
    (ap_uint<64>*)buf_input[0].get(),
    cycle_count);
#line 211 "soda_designs_processed/soda__xcorr/xcorr_host.cpp"
  for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
  {
    int32_t actual_tile_size_dim_0 = (tile_index_dim_0 == tile_count_dim_0-1) ? var_input_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

    for(int32_t j = 9; j < var_tmp3_extent[1] - 9; ++j)
    {
      for(int32_t i = 9; i < actual_tile_size_dim_0 - 9; ++i)
      {



        int32_t p = tile_index_dim_0 * (tile_size_0-kStencilDim0 + 1) + i;
        int32_t q = j;
        int64_t original_offset = p * var_tmp3_stride[0] + q * var_tmp3_stride[1];
        int32_t burst_index_tmp3 = (i + j * tile_size_0 + 4329) / elem_count_per_cycle_tmp3;
        int32_t burst_residue_tmp3 = (i + j * tile_size_0 + 4329) % elem_count_per_cycle_tmp3;
        int64_t tiled_offset_tmp3 = (tile_index_dim_0) * elem_count_aligned_per_tile_o + burst_index_tmp3 * elem_count_per_cycle_tmp3 + burst_residue_tmp3;
        var_tmp3_ptr[original_offset] = buf_tmp3[tiled_offset_tmp3 % bank_count_tmp3].get()[tiled_offset_tmp3 / bank_count_tmp3];
      }
    }
  }
  return 0;
}

}
}



#include <ap_int.h>

int main(int argc, char *argv[])
{
  
  int32_t dims[2] = {480, soda::app::kStencilDim1 + 1};
  if (argc > 2) {
    dims[0] = std::max(dims[0], atoi(argv[2]));
  }
  if (argc > 3) {
    dims[1] = std::max(dims[1], atoi(argv[3]));
  }

  vector<int16_t> data_input(dims[0] * dims[1]);
  int32_t extent_input[2] = {dims[0], dims[1]};
  int32_t stride_input[2] = {1, dims[0]};
  int32_t min_input[2] = {0, 0};
  vector<int16_t> data_tmp1(dims[0] * dims[1]);
  int32_t extent_tmp1[2] = {dims[0], dims[1]};
  int32_t stride_tmp1[2] = {1, dims[0]};
  int32_t min_tmp1[2] = {0, 0};
  vector<int16_t> data_tmp2(dims[0] * dims[1]);
  int32_t extent_tmp2[2] = {dims[0], dims[1]};
  int32_t stride_tmp2[2] = {1, dims[0]};
  int32_t min_tmp2[2] = {0, 0};
  vector<int16_t> data_tmp3(dims[0] * dims[1]);
  int32_t extent_tmp3[2] = {dims[0], dims[1]};
  int32_t stride_tmp3[2] = {1, dims[0]};
  int32_t min_tmp3[2] = {0, 0};



  for(int32_t q = 0; q < dims[1]; ++q)
  {
    for(int32_t p = 0; p < dims[0]; ++p)
    {
      data_input[p * stride_input[0] + q * stride_input[1]] = p+q;
    }
  }

  soda::app::xcorr(
    data_input.data(),
    extent_input,
    stride_input,
    min_input,
    data_tmp3.data(),
    extent_tmp3,
    stride_tmp3,
    min_tmp3);

  int error_count = 0;



  for(int32_t q = 9; q < dims[1] - 9; ++q)
  {
    for(int32_t p = 0; p < dims[0] - 0; ++p)
    {

      data_tmp1[p*stride_tmp1[0]+q*stride_tmp1[1]] = (data_input[(p+0)*stride_input[0] + (q-9)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q-8)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q-7)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q-6)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q-5)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q-4)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q-3)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q-2)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q-1)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+0)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+1)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+2)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+3)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+4)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+5)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+6)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+7)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+8)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+9)*stride_input[1]]);
    }
  }



  for(int32_t q = 9; q < dims[1] - 9; ++q)
  {
    for(int32_t p = 9; p < dims[0] - 9; ++p)
    {

      data_tmp2[p*stride_tmp2[0]+q*stride_tmp2[1]] = (data_tmp1[(p-9)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p-8)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p-7)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p-6)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p-5)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p-4)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p-3)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p-2)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p-1)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p+0)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p+1)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p+2)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p+3)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p+4)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p+5)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p+6)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p+7)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p+8)*stride_tmp1[0] + (q+0)*stride_tmp1[1]] + data_tmp1[(p+9)*stride_tmp1[0] + (q+0)*stride_tmp1[1]]);
    }
  }



  for(int32_t q = 9; q < dims[1] - 9; ++q)
  {
    for(int32_t p = 9; p < dims[0] - 9; ++p)
    {

      int16_t result_tmp3 = (((int32_t)(data_tmp2[(p+0)*stride_tmp2[0] + (q+0)*stride_tmp2[1]]) - data_input[(p+0)*stride_input[0] + (q+0)*stride_input[1]]) * data_input[(p+0)*stride_input[0] + (q+0)*stride_input[1]] / 256);
      int16_t val_fpga = data_tmp3[p * stride_tmp3[0] + q * stride_tmp3[1]];
      int16_t val_cpu = result_tmp3;
      if (val_fpga != val_cpu) {
        if (error_count < 10) {
          fprintf(stderr, "%ld != %ld @(%d, %d)\n", int64_t(val_fpga), int64_t(val_cpu), p, q);
        }
        ++error_count;
      }
    }
  }

  clog << "INFO: " << (error_count ? "FAIL" : "PASS") << "!\n";

  return error_count ? 1 : 0;
}
