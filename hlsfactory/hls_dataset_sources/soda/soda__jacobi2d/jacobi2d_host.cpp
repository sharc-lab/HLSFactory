#line 18 "soda_designs_processed/soda__jacobi2d/jacobi2d_host.cpp"
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
void jacobi2d_kernel(
  ap_uint<64>* bank_1_t0,
  ap_uint<64>* bank_0_t1,
  uint64_t coalesced_data_num);
}

namespace soda {
namespace app {

constexpr int kStencilDim0 = 5;
constexpr int kStencilDim1 = 5;
constexpr int kStencilDistance = 130;
constexpr int kWidth_t1 = 32;
constexpr int kWidth_t0 = 32;


using Type_t1 = float;
using Type_t0 = float;

int jacobi2d(
  const Type_t1* var_t1_ptr,
  const int32_t var_t1_extent[2],
  const int32_t var_t1_stride[2],
  const int32_t var_t1_min[2],
  Type_t0* var_t0_ptr,
  const int32_t var_t0_extent[2],
  const int32_t var_t0_stride[2],
  const int32_t var_t0_min[2],
  // const char* bitstream,
  const int burst_width = 64,
  const int tile_size_0 = 32,
  const int unroll_factor = 2)
{


  int bank_count_t1 = 1;
  int bank_count_t0 = 1;
#line 103 "soda_designs_processed/soda__jacobi2d/jacobi2d_host.cpp"
  auto round_up = [](int64_t a, int64_t b) -> int64_t { return ((a - 1) / b + 1) * b; };


  const int elem_count_per_cycle_t1 = burst_width / kWidth_t1 * bank_count_t1;
  const int elem_count_per_cycle_t0 = burst_width / kWidth_t0 * bank_count_t0;
  int32_t tile_count_dim_0 = (var_t1_extent[0] - kStencilDim0 + 1 - 1) / (tile_size_0 - kStencilDim0 + 1) + 1;
  int64_t tile_count = tile_count_dim_0;


  int64_t elem_count_per_tile = tile_size_0 * var_t1_extent[1];
  int64_t cycle_count_per_tile = (elem_count_per_tile - 1) / elem_count_per_cycle_t1 + 1;
  int64_t elem_count_aligned_per_tile_i = cycle_count_per_tile * elem_count_per_cycle_t1;
  int64_t elem_count_aligned_per_tile_o = cycle_count_per_tile * elem_count_per_cycle_t0;


  int64_t buf_size_t1 = (tile_count * elem_count_aligned_per_tile_i + round_up(kStencilDistance, elem_count_per_cycle_t1)) / bank_count_t1 * sizeof(Type_t1);
  int64_t buf_size_t0 = (tile_count * elem_count_aligned_per_tile_o + round_up(kStencilDistance, elem_count_per_cycle_t0)) / bank_count_t0 * sizeof(Type_t0);


  vector<unique_ptr<Type_t1, decltype(&free)>> buf_t1;
  buf_t1.reserve(bank_count_t1);
  for (int bank = 0; bank < bank_count_t1; ++bank) {
    buf_t1.emplace_back(static_cast<Type_t1*>(aligned_alloc(4096, round_up(buf_size_t1, 4096))), &free);
  }
  vector<unique_ptr<Type_t0, decltype(&free)>> buf_t0;
  buf_t0.reserve(bank_count_t0);
  for (int bank = 0; bank < bank_count_t0; ++bank) {
    buf_t0.emplace_back(static_cast<Type_t0*>(aligned_alloc(4096, round_up(buf_size_t0, 4096))), &free);
  }


  for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
  {
    int32_t actual_tile_size_dim_0 = (tile_index_dim_0==tile_count_dim_0-1) ? var_t1_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

    for(int32_t j = 0; j < var_t1_extent[1]; ++j)
    {
      for(int32_t i = 0; i < actual_tile_size_dim_0; ++i)
      {



        int32_t burst_index = (i + j * tile_size_0) / elem_count_per_cycle_t1;
        int32_t burst_residue = (i + j * tile_size_0) % elem_count_per_cycle_t1;
        int32_t p = tile_index_dim_0 * (tile_size_0 - kStencilDim0) + i;
        int32_t q = j;
        int64_t tiled_offset = (tile_index_dim_0) * elem_count_aligned_per_tile_i + burst_index * elem_count_per_cycle_t1 + burst_residue;
        int64_t original_offset = p * var_t1_stride[0] + q * var_t1_stride[1];
        buf_t1[tiled_offset % bank_count_t1].get()[tiled_offset / bank_count_t1] = var_t1_ptr[std::max(int64_t(0), original_offset - 0)];
      }
    }
  }

  clog << "INFO: tile_count[0] = " << tile_count_dim_0 << ", tile_size[0] = " << tile_size_0 << endl;
  clog << "INFO: t1.extent[0] = " << var_t1_extent[0] << ", t1.extent[1] = " << var_t1_extent[1] << "" << endl;
  clog << "INFO: t1.stride[0] = " << var_t1_stride[0] << ", t1.stride[1] = " << var_t1_stride[1] << "" << endl;
  clog << "INFO: t1.min[0] = " << var_t1_min[0] << ", t1.min[1] = " << var_t1_min[1] << "" << endl;
  clog << "INFO: t0.extent[0] = " << var_t0_extent[0] << ", t0.extent[1] = " << var_t0_extent[1] << "" << endl;
  clog << "INFO: t0.stride[0] = " << var_t0_stride[0] << ", t0.stride[1] = " << var_t0_stride[1] << "" << endl;
  clog << "INFO: t0.min[0] = " << var_t0_min[0] << ", t0.min[1] = " << var_t0_min[1] << "" << endl;

  int64_t tile_data_count = ((int64_t(var_t1_extent[1]) * tile_size_0 - 1) / elem_count_per_cycle_t1 + 1) * elem_count_per_cycle_t1 / unroll_factor;
  int64_t cycle_count = ((int64_t(var_t1_extent[1]) * tile_size_0 * tile_count_dim_0 + kStencilDistance - 1) / elem_count_per_cycle_t1 + 1);
  clog << "INFO: tile_data_count = " << tile_data_count << ", cycle_count = " << cycle_count << endl;


  ::jacobi2d_kernel(
    (ap_uint<64>*)buf_t0[0].get(),
    (ap_uint<64>*)buf_t1[0].get(),
    cycle_count);
#line 209 "soda_designs_processed/soda__jacobi2d/jacobi2d_host.cpp"
  for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
  {
    int32_t actual_tile_size_dim_0 = (tile_index_dim_0 == tile_count_dim_0-1) ? var_t1_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

    for(int32_t j = 2; j < var_t0_extent[1] - 2; ++j)
    {
      for(int32_t i = 2; i < actual_tile_size_dim_0 - 2; ++i)
      {



        int32_t p = tile_index_dim_0 * (tile_size_0-kStencilDim0 + 1) + i;
        int32_t q = j;
        int64_t original_offset = p * var_t0_stride[0] + q * var_t0_stride[1];
        int32_t burst_index_t0 = (i + j * tile_size_0 + 64) / elem_count_per_cycle_t0;
        int32_t burst_residue_t0 = (i + j * tile_size_0 + 64) % elem_count_per_cycle_t0;
        int64_t tiled_offset_t0 = (tile_index_dim_0) * elem_count_aligned_per_tile_o + burst_index_t0 * elem_count_per_cycle_t0 + burst_residue_t0;
        var_t0_ptr[original_offset] = buf_t0[tiled_offset_t0 % bank_count_t0].get()[tiled_offset_t0 / bank_count_t0];
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
  
  int32_t dims[2] = {32, soda::app::kStencilDim1 + 1};
  if (argc > 2) {
    dims[0] = std::max(dims[0], atoi(argv[2]));
  }
  if (argc > 3) {
    dims[1] = std::max(dims[1], atoi(argv[3]));
  }

  vector<float> data_t1(dims[0] * dims[1]);
  int32_t extent_t1[2] = {dims[0], dims[1]};
  int32_t stride_t1[2] = {1, dims[0]};
  int32_t min_t1[2] = {0, 0};
  vector<float> data_t1_iter1(dims[0] * dims[1]);
  int32_t extent_t1_iter1[2] = {dims[0], dims[1]};
  int32_t stride_t1_iter1[2] = {1, dims[0]};
  int32_t min_t1_iter1[2] = {0, 0};
  vector<float> data_t0(dims[0] * dims[1]);
  int32_t extent_t0[2] = {dims[0], dims[1]};
  int32_t stride_t0[2] = {1, dims[0]};
  int32_t min_t0[2] = {0, 0};

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);



  for(int32_t q = 0; q < dims[1]; ++q)
  {
    for(int32_t p = 0; p < dims[0]; ++p)
    {
      data_t1[p * stride_t1[0] + q * stride_t1[1]] = distribution(generator);
    }
  }

  soda::app::jacobi2d(
    data_t1.data(),
    extent_t1,
    stride_t1,
    min_t1,
    data_t0.data(),
    extent_t0,
    stride_t0,
    min_t0);

  int error_count = 0;



  for(int32_t q = 1; q < dims[1] - 1; ++q)
  {
    for(int32_t p = 1; p < dims[0] - 1; ++p)
    {

      data_t1_iter1[p*stride_t1_iter1[0]+q*stride_t1_iter1[1]] = ((data_t1[(p+0)*stride_t1[0] + (q+1)*stride_t1[1]] + data_t1[(p+1)*stride_t1[0] + (q+0)*stride_t1[1]] + data_t1[(p+0)*stride_t1[0] + (q+0)*stride_t1[1]] + data_t1[(p+0)*stride_t1[0] + (q-1)*stride_t1[1]] + data_t1[(p-1)*stride_t1[0] + (q+0)*stride_t1[1]]) * 0.2f);
    }
  }



  for(int32_t q = 2; q < dims[1] - 2; ++q)
  {
    for(int32_t p = 2; p < dims[0] - 2; ++p)
    {

      float result_t0 = ((data_t1_iter1[(p+0)*stride_t1_iter1[0] + (q+1)*stride_t1_iter1[1]] + data_t1_iter1[(p+1)*stride_t1_iter1[0] + (q+0)*stride_t1_iter1[1]] + data_t1_iter1[(p+0)*stride_t1_iter1[0] + (q+0)*stride_t1_iter1[1]] + data_t1_iter1[(p+0)*stride_t1_iter1[0] + (q-1)*stride_t1_iter1[1]] + data_t1_iter1[(p-1)*stride_t1_iter1[0] + (q+0)*stride_t1_iter1[1]]) * 0.2f);
      float val_fpga = data_t0[p * stride_t0[0] + q * stride_t0[1]];
      float val_cpu = result_t0;
      double threshold = 0.00001;
      if (nullptr != getenv("THRESHOLD")) {
        threshold = atof(getenv("THRESHOLD"));
      }
      threshold *= threshold;
      if (double(val_fpga-val_cpu) * double(val_fpga-val_cpu) > threshold && double(val_fpga-val_cpu) * double(val_fpga-val_cpu) / (double(val_cpu) * double(val_cpu)) > threshold) {
        fprintf(stderr, "%lf != %lf @(%d, %d)\n", double(val_fpga), double(val_cpu), p, q);
        ++error_count;
      }
    }
  }

  clog << "INFO: " << (error_count ? "FAIL" : "PASS") << "!\n";

  return error_count ? 1 : 0;
}
