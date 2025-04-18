#line 28 "soda_designs_processed/soda__denoise3d/denoise3d_host.cpp"
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
void denoise3d_kernel(
  ap_uint<64>* bank_2_output,
  ap_uint<64>* bank_0_f,
  ap_uint<64>* bank_1_u,
  uint64_t coalesced_data_num);
}

namespace soda {
namespace app {

constexpr int kStencilDim0 = 5;
constexpr int kStencilDim1 = 5;
constexpr int kStencilDim2 = 5;
constexpr int kStencilDistance = 4162;
constexpr int kWidth_f = 32;
constexpr int kWidth_u = 32;
constexpr int kWidth_output = 32;


using Type_f = float;
using Type_u = float;
using Type_output = float;

int denoise3d(
  const Type_f* var_f_ptr,
  const int32_t var_f_extent[3],
  const int32_t var_f_stride[3],
  const int32_t var_f_min[3],
  const Type_u* var_u_ptr,
  const int32_t var_u_extent[3],
  const int32_t var_u_stride[3],
  const int32_t var_u_min[3],
  Type_output* var_output_ptr,
  const int32_t var_output_extent[3],
  const int32_t var_output_stride[3],
  const int32_t var_output_min[3],
  // const char* bitstream,
  const int burst_width = 64,
  const int tile_size_0 = 32,
  const int tile_size_1 = 32,
  const int unroll_factor = 2)
{


  int bank_count_f = 1;
  int bank_count_u = 1;
  int bank_count_output = 1;
#line 126 "soda_designs_processed/soda__denoise3d/denoise3d_host.cpp"
  auto round_up = [](int64_t a, int64_t b) -> int64_t { return ((a - 1) / b + 1) * b; };


  const int elem_count_per_cycle_f = burst_width / kWidth_f * bank_count_f;
  const int elem_count_per_cycle_u = burst_width / kWidth_u * bank_count_u;
  const int elem_count_per_cycle_output = burst_width / kWidth_output * bank_count_output;
  int32_t tile_count_dim_0 = (var_f_extent[0] - kStencilDim0 + 1 - 1) / (tile_size_0 - kStencilDim0 + 1) + 1;
  int32_t tile_count_dim_1 = (var_f_extent[1] - kStencilDim1 + 1 - 1) / (tile_size_1 - kStencilDim1 + 1) + 1;
  int64_t tile_count = tile_count_dim_0 * tile_count_dim_1;


  int64_t elem_count_per_tile = tile_size_0 * tile_size_1 * var_f_extent[2];
  int64_t cycle_count_per_tile = (elem_count_per_tile - 1) / elem_count_per_cycle_f + 1;
  int64_t elem_count_aligned_per_tile_i = cycle_count_per_tile * elem_count_per_cycle_f;
  int64_t elem_count_aligned_per_tile_o = cycle_count_per_tile * elem_count_per_cycle_output;


  int64_t buf_size_f = (tile_count * elem_count_aligned_per_tile_i + round_up(kStencilDistance, elem_count_per_cycle_f)) / bank_count_f * sizeof(Type_f);
  int64_t buf_size_u = (tile_count * elem_count_aligned_per_tile_i + round_up(kStencilDistance, elem_count_per_cycle_u)) / bank_count_u * sizeof(Type_u);
  int64_t buf_size_output = (tile_count * elem_count_aligned_per_tile_o + round_up(kStencilDistance, elem_count_per_cycle_output)) / bank_count_output * sizeof(Type_output);


  vector<unique_ptr<Type_f, decltype(&free)>> buf_f;
  buf_f.reserve(bank_count_f);
  for (int bank = 0; bank < bank_count_f; ++bank) {
    buf_f.emplace_back(static_cast<Type_f*>(aligned_alloc(4096, round_up(buf_size_f, 4096))), &free);
  }
  vector<unique_ptr<Type_u, decltype(&free)>> buf_u;
  buf_u.reserve(bank_count_u);
  for (int bank = 0; bank < bank_count_u; ++bank) {
    buf_u.emplace_back(static_cast<Type_u*>(aligned_alloc(4096, round_up(buf_size_u, 4096))), &free);
  }
  vector<unique_ptr<Type_output, decltype(&free)>> buf_output;
  buf_output.reserve(bank_count_output);
  for (int bank = 0; bank < bank_count_output; ++bank) {
    buf_output.emplace_back(static_cast<Type_output*>(aligned_alloc(4096, round_up(buf_size_output, 4096))), &free);
  }


  for(int32_t tile_index_dim_1 = 0; tile_index_dim_1 < tile_count_dim_1; ++tile_index_dim_1)
  {
    int32_t actual_tile_size_dim_1 = (tile_index_dim_1==tile_count_dim_1-1) ? var_f_extent[1] - (tile_size_1 - kStencilDim1 + 1) * tile_index_dim_1 : tile_size_1;
    for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
    {
      int32_t actual_tile_size_dim_0 = (tile_index_dim_0==tile_count_dim_0-1) ? var_f_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

      for(int32_t k = 0; k < var_f_extent[2]; ++k)
      {
        for(int32_t j = 0; j < actual_tile_size_dim_1; ++j)
        {
          for(int32_t i = 0; i < actual_tile_size_dim_0; ++i)
          {



            int32_t burst_index = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1) / elem_count_per_cycle_f;
            int32_t burst_residue = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1) % elem_count_per_cycle_f;
            int32_t p = tile_index_dim_0 * (tile_size_0 - kStencilDim0) + i;
            int32_t q = tile_index_dim_1 * (tile_size_1 - kStencilDim1) + j;
            int32_t r = k;
            int64_t tiled_offset = (tile_index_dim_0 + tile_count_dim_0 * tile_index_dim_1) * elem_count_aligned_per_tile_i + burst_index * elem_count_per_cycle_f + burst_residue;
            int64_t original_offset = p * var_f_stride[0] + q * var_f_stride[1] + r * var_f_stride[2];
            buf_f[tiled_offset % bank_count_f].get()[tiled_offset / bank_count_f] = var_f_ptr[std::max(int64_t(0), original_offset - 2048)];
            buf_u[tiled_offset % bank_count_u].get()[tiled_offset / bank_count_u] = var_u_ptr[std::max(int64_t(0), original_offset - 0)];
          }
        }
      }
    }
  }

  clog << "INFO: tile_count[0] = " << tile_count_dim_0 << ", tile_size[0] = " << tile_size_0 << endl;
  clog << "INFO: tile_count[1] = " << tile_count_dim_1 << ", tile_size[1] = " << tile_size_1 << endl;
  clog << "INFO: f.extent[0] = " << var_f_extent[0] << ", f.extent[1] = " << var_f_extent[1] << ", f.extent[2] = " << var_f_extent[2] << "" << endl;
  clog << "INFO: f.stride[0] = " << var_f_stride[0] << ", f.stride[1] = " << var_f_stride[1] << ", f.stride[2] = " << var_f_stride[2] << "" << endl;
  clog << "INFO: f.min[0] = " << var_f_min[0] << ", f.min[1] = " << var_f_min[1] << ", f.min[2] = " << var_f_min[2] << "" << endl;
  clog << "INFO: u.extent[0] = " << var_u_extent[0] << ", u.extent[1] = " << var_u_extent[1] << ", u.extent[2] = " << var_u_extent[2] << "" << endl;
  clog << "INFO: u.stride[0] = " << var_u_stride[0] << ", u.stride[1] = " << var_u_stride[1] << ", u.stride[2] = " << var_u_stride[2] << "" << endl;
  clog << "INFO: u.min[0] = " << var_u_min[0] << ", u.min[1] = " << var_u_min[1] << ", u.min[2] = " << var_u_min[2] << "" << endl;
  clog << "INFO: output.extent[0] = " << var_output_extent[0] << ", output.extent[1] = " << var_output_extent[1] << ", output.extent[2] = " << var_output_extent[2] << "" << endl;
  clog << "INFO: output.stride[0] = " << var_output_stride[0] << ", output.stride[1] = " << var_output_stride[1] << ", output.stride[2] = " << var_output_stride[2] << "" << endl;
  clog << "INFO: output.min[0] = " << var_output_min[0] << ", output.min[1] = " << var_output_min[1] << ", output.min[2] = " << var_output_min[2] << "" << endl;

  int64_t tile_data_count = ((int64_t(var_f_extent[2]) * tile_size_0 * tile_size_1 - 1) / elem_count_per_cycle_f + 1) * elem_count_per_cycle_f / unroll_factor;
  int64_t cycle_count = ((int64_t(var_f_extent[2]) * tile_size_0 * tile_size_1 * tile_count_dim_0 * tile_count_dim_1 + kStencilDistance - 1) / elem_count_per_cycle_f + 1);
  clog << "INFO: tile_data_count = " << tile_data_count << ", cycle_count = " << cycle_count << endl;


  ::denoise3d_kernel(
    (ap_uint<64>*)buf_output[0].get(),
    (ap_uint<64>*)buf_f[0].get(),
    (ap_uint<64>*)buf_u[0].get(),
    cycle_count);
#line 262 "soda_designs_processed/soda__denoise3d/denoise3d_host.cpp"
  for(int32_t tile_index_dim_1 = 0; tile_index_dim_1 < tile_count_dim_1; ++tile_index_dim_1)
  {
    int32_t actual_tile_size_dim_1 = (tile_index_dim_1 == tile_count_dim_1-1) ? var_f_extent[1] - (tile_size_1 - kStencilDim1 + 1) * tile_index_dim_1 : tile_size_1;
    for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
    {
      int32_t actual_tile_size_dim_0 = (tile_index_dim_0 == tile_count_dim_0-1) ? var_f_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

      for(int32_t k = 0; k < var_output_extent[2] - 0; ++k)
      {
        for(int32_t j = 0; j < actual_tile_size_dim_1 - 0; ++j)
        {
          for(int32_t i = 0; i < actual_tile_size_dim_0 - 0; ++i)
          {



            int32_t p = tile_index_dim_0 * (tile_size_0-kStencilDim0 + 1) + i;
            int32_t q = tile_index_dim_1 * (tile_size_1-kStencilDim1 + 1) + j;
            int32_t r = k;
            int64_t original_offset = p * var_output_stride[0] + q * var_output_stride[1] + r * var_output_stride[2];
            int32_t burst_index_output = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1 + 2048) / elem_count_per_cycle_output;
            int32_t burst_residue_output = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1 + 2048) % elem_count_per_cycle_output;
            int64_t tiled_offset_output = (tile_index_dim_0+tile_count_dim_0 * tile_index_dim_1) * elem_count_aligned_per_tile_o + burst_index_output * elem_count_per_cycle_output + burst_residue_output;
            var_output_ptr[original_offset] = buf_output[tiled_offset_output % bank_count_output].get()[tiled_offset_output / bank_count_output];
          }
        }
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
  if (argc < 2 || argc > 5) {
    clog << "Usage: \n  " << argv[0] << " <xclbin> [input dimension 0] [input dimension 1] [input dimension 2]" << endl;
    return 2;
  }
  const char* xclbin = argv[1];
  int32_t dims[3] = {32, 32, soda::app::kStencilDim2 + 1};
  if (argc > 2) {
    dims[0] = std::max(dims[0], atoi(argv[2]));
  }
  if (argc > 3) {
    dims[1] = std::max(dims[1], atoi(argv[3]));
  }
  if (argc > 4) {
    dims[2] = std::max(dims[2], atoi(argv[4]));
  }

  vector<float> data_f(dims[0] * dims[1] * dims[2]);
  int32_t extent_f[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_f[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_f[3] = {0, 0, 0};
  vector<float> data_u(dims[0] * dims[1] * dims[2]);
  int32_t extent_u[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_u[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_u[3] = {0, 0, 0};
  vector<float> data_diff_u(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_u[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_u[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_u[3] = {0, 0, 0};
  vector<float> data_diff_d(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_d[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_d[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_d[3] = {0, 0, 0};
  vector<float> data_diff_l(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_l[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_l[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_l[3] = {0, 0, 0};
  vector<float> data_diff_r(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_r[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_r[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_r[3] = {0, 0, 0};
  vector<float> data_diff_i(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_i[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_i[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_i[3] = {0, 0, 0};
  vector<float> data_diff_o(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_o[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_o[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_o[3] = {0, 0, 0};
  vector<float> data_g(dims[0] * dims[1] * dims[2]);
  int32_t extent_g[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_g[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_g[3] = {0, 0, 0};
  vector<float> data_r0(dims[0] * dims[1] * dims[2]);
  int32_t extent_r0[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_r0[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_r0[3] = {0, 0, 0};
  vector<float> data_r1(dims[0] * dims[1] * dims[2]);
  int32_t extent_r1[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_r1[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_r1[3] = {0, 0, 0};
  vector<float> data_output(dims[0] * dims[1] * dims[2]);
  int32_t extent_output[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_output[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_output[3] = {0, 0, 0};

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);



  for(int32_t r = 0; r < dims[2]; ++r)
  {
    for(int32_t q = 0; q < dims[1]; ++q)
    {
      for(int32_t p = 0; p < dims[0]; ++p)
      {
        data_f[p * stride_f[0] + q * stride_f[1] + r * stride_f[2]] = distribution(generator);
      }
    }
  }



  for(int32_t r = 0; r < dims[2]; ++r)
  {
    for(int32_t q = 0; q < dims[1]; ++q)
    {
      for(int32_t p = 0; p < dims[0]; ++p)
      {
        data_u[p * stride_u[0] + q * stride_u[1] + r * stride_u[2]] = distribution(generator);
      }
    }
  }

  soda::app::denoise3d(
    data_f.data(),
    extent_f,
    stride_f,
    min_f,
    data_u.data(),
    extent_u,
    stride_u,
    min_u,
    data_output.data(),
    extent_output,
    stride_output,
    min_output);

  int error_count = 0;



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 1; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_diff_d[p*stride_diff_d[0]+q*stride_diff_d[1]+r*stride_diff_d[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p+0)*stride_u[0] + (q+1)*stride_u[1] + (r+0)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 1; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_diff_i[p*stride_diff_i[0]+q*stride_diff_i[1]+r*stride_diff_i[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r-1)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 1; p < dims[0] - 0; ++p)
      {

        data_diff_l[p*stride_diff_l[0]+q*stride_diff_l[1]+r*stride_diff_l[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p-1)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 1; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_diff_o[p*stride_diff_o[0]+q*stride_diff_o[1]+r*stride_diff_o[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+1)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 1; ++p)
      {

        data_diff_r[p*stride_diff_r[0]+q*stride_diff_r[1]+r*stride_diff_r[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p+1)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 1; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_diff_u[p*stride_diff_u[0]+q*stride_diff_u[1]+r*stride_diff_u[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p+0)*stride_u[0] + (q-1)*stride_u[1] + (r+0)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_r0[p*stride_r0[0]+q*stride_r0[1]+r*stride_r0[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] * data_f[(p+0)*stride_f[0] + (q+0)*stride_f[1] + (r+0)*stride_f[2]] * (1.0f / 0.03f));
      }
    }
  }



  for(int32_t r = 1; r < dims[2] - 1; ++r)
  {
    for(int32_t q = 1; q < dims[1] - 1; ++q)
    {
      for(int32_t p = 1; p < dims[0] - 1; ++p)
      {

        data_g[p*stride_g[0]+q*stride_g[1]+r*stride_g[2]] = (1.0f / sqrt((0.00005f + (data_diff_u[(p+0)*stride_diff_u[0] + (q+0)*stride_diff_u[1] + (r+0)*stride_diff_u[2]] * data_diff_u[(p+0)*stride_diff_u[0] + (q+0)*stride_diff_u[1] + (r+0)*stride_diff_u[2]]) + (data_diff_d[(p+0)*stride_diff_d[0] + (q+0)*stride_diff_d[1] + (r+0)*stride_diff_d[2]] * data_diff_d[(p+0)*stride_diff_d[0] + (q+0)*stride_diff_d[1] + (r+0)*stride_diff_d[2]]) + (data_diff_l[(p+0)*stride_diff_l[0] + (q+0)*stride_diff_l[1] + (r+0)*stride_diff_l[2]] * data_diff_l[(p+0)*stride_diff_l[0] + (q+0)*stride_diff_l[1] + (r+0)*stride_diff_l[2]]) + (data_diff_r[(p+0)*stride_diff_r[0] + (q+0)*stride_diff_r[1] + (r+0)*stride_diff_r[2]] * data_diff_r[(p+0)*stride_diff_r[0] + (q+0)*stride_diff_r[1] + (r+0)*stride_diff_r[2]]) + (data_diff_i[(p+0)*stride_diff_i[0] + (q+0)*stride_diff_i[1] + (r+0)*stride_diff_i[2]] * data_diff_i[(p+0)*stride_diff_i[0] + (q+0)*stride_diff_i[1] + (r+0)*stride_diff_i[2]]) + (data_diff_o[(p+0)*stride_diff_o[0] + (q+0)*stride_diff_o[1] + (r+0)*stride_diff_o[2]] * data_diff_o[(p+0)*stride_diff_o[0] + (q+0)*stride_diff_o[1] + (r+0)*stride_diff_o[2]]))));
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_r1[p*stride_r1[0]+q*stride_r1[1]+r*stride_r1[2]] = ((data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]] * (2.38944f + (data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]] * (0.950037f + data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]])))) / (4.65314f + (data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]] * (2.57541f + (data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]] * (1.48937f + data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]]))))));
      }
    }
  }



  for(int32_t r = 2; r < dims[2] - 2; ++r)
  {
    for(int32_t q = 2; q < dims[1] - 2; ++q)
    {
      for(int32_t p = 2; p < dims[0] - 2; ++p)
      {

        float result_output = ((data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] + (5.0f * ((data_u[(p+1)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] * data_g[(p+1)*stride_g[0] + (q+0)*stride_g[1] + (r+0)*stride_g[2]]) + (data_u[(p-1)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] * data_g[(p-1)*stride_g[0] + (q+0)*stride_g[1] + (r+0)*stride_g[2]]) + (data_u[(p+0)*stride_u[0] + (q+1)*stride_u[1] + (r+0)*stride_u[2]] * data_g[(p+0)*stride_g[0] + (q+1)*stride_g[1] + (r+0)*stride_g[2]]) + (data_u[(p+0)*stride_u[0] + (q-1)*stride_u[1] + (r+0)*stride_u[2]] * data_g[(p+0)*stride_g[0] + (q-1)*stride_g[1] + (r+0)*stride_g[2]]) + (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+1)*stride_u[2]] * data_g[(p+0)*stride_g[0] + (q+0)*stride_g[1] + (r+1)*stride_g[2]]) + (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r-1)*stride_u[2]] * data_g[(p+0)*stride_g[0] + (q+0)*stride_g[1] + (r-1)*stride_g[2]]) + ((1.0f / 0.03f) * data_f[(p+0)*stride_f[0] + (q+0)*stride_f[1] + (r+0)*stride_f[2]] * data_r1[(p+0)*stride_r1[0] + (q+0)*stride_r1[1] + (r+0)*stride_r1[2]])))) / (1.0f + (5.0f * (data_g[(p+1)*stride_g[0] + (q+0)*stride_g[1] + (r+0)*stride_g[2]] + data_g[(p-1)*stride_g[0] + (q+0)*stride_g[1] + (r+0)*stride_g[2]] + data_g[(p+0)*stride_g[0] + (q+1)*stride_g[1] + (r+0)*stride_g[2]] + data_g[(p+0)*stride_g[0] + (q-1)*stride_g[1] + (r+0)*stride_g[2]] + data_g[(p+0)*stride_g[0] + (q+0)*stride_g[1] + (r+1)*stride_g[2]] + data_g[(p+0)*stride_g[0] + (q+0)*stride_g[1] + (r-1)*stride_g[2]] + (1.0f / 0.03f)))));
        float val_fpga = data_output[p * stride_output[0] + q * stride_output[1] + r * stride_output[2]];
        float val_cpu = result_output;
        double threshold = 0.00001;
        if (nullptr != getenv("THRESHOLD")) {
          threshold = atof(getenv("THRESHOLD"));
        }
        threshold *= threshold;
        if (double(val_fpga-val_cpu) * double(val_fpga-val_cpu) > threshold && double(val_fpga-val_cpu) * double(val_fpga-val_cpu) / (double(val_cpu) * double(val_cpu)) > threshold) {
          fprintf(stderr, "%lf != %lf @(%d, %d, %d)\n", double(val_fpga), double(val_cpu), p, q, r);
          ++error_count;
        }
      }
    }
  }

  clog << "INFO: " << (error_count ? "FAIL" : "PASS") << "!\n";

  return error_count ? 1 : 0;
}ssed/soda__denoise3d/denoise3d_host.cpp"
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
void denoise3d_kernel(
  ap_uint<64>* bank_2_output,
  ap_uint<64>* bank_0_f,
  ap_uint<64>* bank_1_u,
  uint64_t coalesced_data_num);
}

namespace soda {
namespace app {

constexpr int kStencilDim0 = 5;
constexpr int kStencilDim1 = 5;
constexpr int kStencilDim2 = 5;
constexpr int kStencilDistance = 4162;
constexpr int kWidth_f = 32;
constexpr int kWidth_u = 32;
constexpr int kWidth_output = 32;


using Type_f = float;
using Type_u = float;
using Type_output = float;

int denoise3d(
  const Type_f* var_f_ptr,
  const int32_t var_f_extent[3],
  const int32_t var_f_stride[3],
  const int32_t var_f_min[3],
  const Type_u* var_u_ptr,
  const int32_t var_u_extent[3],
  const int32_t var_u_stride[3],
  const int32_t var_u_min[3],
  Type_output* var_output_ptr,
  const int32_t var_output_extent[3],
  const int32_t var_output_stride[3],
  const int32_t var_output_min[3],
  // const char* bitstream,
  const int burst_width = 64,
  const int tile_size_0 = 32,
  const int tile_size_1 = 32,
  const int unroll_factor = 2)
{


  int bank_count_f = 1;
  int bank_count_u = 1;
  int bank_count_output = 1;
#line 126 "soda_designs_processed/soda__denoise3d/denoise3d_host.cpp"
  auto round_up = [](int64_t a, int64_t b) -> int64_t { return ((a - 1) / b + 1) * b; };


  const int elem_count_per_cycle_f = burst_width / kWidth_f * bank_count_f;
  const int elem_count_per_cycle_u = burst_width / kWidth_u * bank_count_u;
  const int elem_count_per_cycle_output = burst_width / kWidth_output * bank_count_output;
  int32_t tile_count_dim_0 = (var_f_extent[0] - kStencilDim0 + 1 - 1) / (tile_size_0 - kStencilDim0 + 1) + 1;
  int32_t tile_count_dim_1 = (var_f_extent[1] - kStencilDim1 + 1 - 1) / (tile_size_1 - kStencilDim1 + 1) + 1;
  int64_t tile_count = tile_count_dim_0 * tile_count_dim_1;


  int64_t elem_count_per_tile = tile_size_0 * tile_size_1 * var_f_extent[2];
  int64_t cycle_count_per_tile = (elem_count_per_tile - 1) / elem_count_per_cycle_f + 1;
  int64_t elem_count_aligned_per_tile_i = cycle_count_per_tile * elem_count_per_cycle_f;
  int64_t elem_count_aligned_per_tile_o = cycle_count_per_tile * elem_count_per_cycle_output;


  int64_t buf_size_f = (tile_count * elem_count_aligned_per_tile_i + round_up(kStencilDistance, elem_count_per_cycle_f)) / bank_count_f * sizeof(Type_f);
  int64_t buf_size_u = (tile_count * elem_count_aligned_per_tile_i + round_up(kStencilDistance, elem_count_per_cycle_u)) / bank_count_u * sizeof(Type_u);
  int64_t buf_size_output = (tile_count * elem_count_aligned_per_tile_o + round_up(kStencilDistance, elem_count_per_cycle_output)) / bank_count_output * sizeof(Type_output);


  vector<unique_ptr<Type_f, decltype(&free)>> buf_f;
  buf_f.reserve(bank_count_f);
  for (int bank = 0; bank < bank_count_f; ++bank) {
    buf_f.emplace_back(static_cast<Type_f*>(aligned_alloc(4096, round_up(buf_size_f, 4096))), &free);
  }
  vector<unique_ptr<Type_u, decltype(&free)>> buf_u;
  buf_u.reserve(bank_count_u);
  for (int bank = 0; bank < bank_count_u; ++bank) {
    buf_u.emplace_back(static_cast<Type_u*>(aligned_alloc(4096, round_up(buf_size_u, 4096))), &free);
  }
  vector<unique_ptr<Type_output, decltype(&free)>> buf_output;
  buf_output.reserve(bank_count_output);
  for (int bank = 0; bank < bank_count_output; ++bank) {
    buf_output.emplace_back(static_cast<Type_output*>(aligned_alloc(4096, round_up(buf_size_output, 4096))), &free);
  }


  for(int32_t tile_index_dim_1 = 0; tile_index_dim_1 < tile_count_dim_1; ++tile_index_dim_1)
  {
    int32_t actual_tile_size_dim_1 = (tile_index_dim_1==tile_count_dim_1-1) ? var_f_extent[1] - (tile_size_1 - kStencilDim1 + 1) * tile_index_dim_1 : tile_size_1;
    for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
    {
      int32_t actual_tile_size_dim_0 = (tile_index_dim_0==tile_count_dim_0-1) ? var_f_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

      for(int32_t k = 0; k < var_f_extent[2]; ++k)
      {
        for(int32_t j = 0; j < actual_tile_size_dim_1; ++j)
        {
          for(int32_t i = 0; i < actual_tile_size_dim_0; ++i)
          {



            int32_t burst_index = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1) / elem_count_per_cycle_f;
            int32_t burst_residue = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1) % elem_count_per_cycle_f;
            int32_t p = tile_index_dim_0 * (tile_size_0 - kStencilDim0) + i;
            int32_t q = tile_index_dim_1 * (tile_size_1 - kStencilDim1) + j;
            int32_t r = k;
            int64_t tiled_offset = (tile_index_dim_0 + tile_count_dim_0 * tile_index_dim_1) * elem_count_aligned_per_tile_i + burst_index * elem_count_per_cycle_f + burst_residue;
            int64_t original_offset = p * var_f_stride[0] + q * var_f_stride[1] + r * var_f_stride[2];
            buf_f[tiled_offset % bank_count_f].get()[tiled_offset / bank_count_f] = var_f_ptr[std::max(int64_t(0), original_offset - 2048)];
            buf_u[tiled_offset % bank_count_u].get()[tiled_offset / bank_count_u] = var_u_ptr[std::max(int64_t(0), original_offset - 0)];
          }
        }
      }
    }
  }

  clog << "INFO: tile_count[0] = " << tile_count_dim_0 << ", tile_size[0] = " << tile_size_0 << endl;
  clog << "INFO: tile_count[1] = " << tile_count_dim_1 << ", tile_size[1] = " << tile_size_1 << endl;
  clog << "INFO: f.extent[0] = " << var_f_extent[0] << ", f.extent[1] = " << var_f_extent[1] << ", f.extent[2] = " << var_f_extent[2] << "" << endl;
  clog << "INFO: f.stride[0] = " << var_f_stride[0] << ", f.stride[1] = " << var_f_stride[1] << ", f.stride[2] = " << var_f_stride[2] << "" << endl;
  clog << "INFO: f.min[0] = " << var_f_min[0] << ", f.min[1] = " << var_f_min[1] << ", f.min[2] = " << var_f_min[2] << "" << endl;
  clog << "INFO: u.extent[0] = " << var_u_extent[0] << ", u.extent[1] = " << var_u_extent[1] << ", u.extent[2] = " << var_u_extent[2] << "" << endl;
  clog << "INFO: u.stride[0] = " << var_u_stride[0] << ", u.stride[1] = " << var_u_stride[1] << ", u.stride[2] = " << var_u_stride[2] << "" << endl;
  clog << "INFO: u.min[0] = " << var_u_min[0] << ", u.min[1] = " << var_u_min[1] << ", u.min[2] = " << var_u_min[2] << "" << endl;
  clog << "INFO: output.extent[0] = " << var_output_extent[0] << ", output.extent[1] = " << var_output_extent[1] << ", output.extent[2] = " << var_output_extent[2] << "" << endl;
  clog << "INFO: output.stride[0] = " << var_output_stride[0] << ", output.stride[1] = " << var_output_stride[1] << ", output.stride[2] = " << var_output_stride[2] << "" << endl;
  clog << "INFO: output.min[0] = " << var_output_min[0] << ", output.min[1] = " << var_output_min[1] << ", output.min[2] = " << var_output_min[2] << "" << endl;

  int64_t tile_data_count = ((int64_t(var_f_extent[2]) * tile_size_0 * tile_size_1 - 1) / elem_count_per_cycle_f + 1) * elem_count_per_cycle_f / unroll_factor;
  int64_t cycle_count = ((int64_t(var_f_extent[2]) * tile_size_0 * tile_size_1 * tile_count_dim_0 * tile_count_dim_1 + kStencilDistance - 1) / elem_count_per_cycle_f + 1);
  clog << "INFO: tile_data_count = " << tile_data_count << ", cycle_count = " << cycle_count << endl;


  ::denoise3d_kernel(
    (ap_uint<64>*)buf_output[0].get(),
    (ap_uint<64>*)buf_f[0].get(),
    (ap_uint<64>*)buf_u[0].get(),
    cycle_count);
#line 262 "soda_designs_processed/soda__denoise3d/denoise3d_host.cpp"
  for(int32_t tile_index_dim_1 = 0; tile_index_dim_1 < tile_count_dim_1; ++tile_index_dim_1)
  {
    int32_t actual_tile_size_dim_1 = (tile_index_dim_1 == tile_count_dim_1-1) ? var_f_extent[1] - (tile_size_1 - kStencilDim1 + 1) * tile_index_dim_1 : tile_size_1;
    for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
    {
      int32_t actual_tile_size_dim_0 = (tile_index_dim_0 == tile_count_dim_0-1) ? var_f_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

      for(int32_t k = 0; k < var_output_extent[2] - 0; ++k)
      {
        for(int32_t j = 0; j < actual_tile_size_dim_1 - 0; ++j)
        {
          for(int32_t i = 0; i < actual_tile_size_dim_0 - 0; ++i)
          {



            int32_t p = tile_index_dim_0 * (tile_size_0-kStencilDim0 + 1) + i;
            int32_t q = tile_index_dim_1 * (tile_size_1-kStencilDim1 + 1) + j;
            int32_t r = k;
            int64_t original_offset = p * var_output_stride[0] + q * var_output_stride[1] + r * var_output_stride[2];
            int32_t burst_index_output = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1 + 2048) / elem_count_per_cycle_output;
            int32_t burst_residue_output = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1 + 2048) % elem_count_per_cycle_output;
            int64_t tiled_offset_output = (tile_index_dim_0+tile_count_dim_0 * tile_index_dim_1) * elem_count_aligned_per_tile_o + burst_index_output * elem_count_per_cycle_output + burst_residue_output;
            var_output_ptr[original_offset] = buf_output[tiled_offset_output % bank_count_output].get()[tiled_offset_output / bank_count_output];
          }
        }
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
  if (argc < 2 || argc > 5) {
    clog << "Usage: \n  " << argv[0] << " <xclbin> [input dimension 0] [input dimension 1] [input dimension 2]" << endl;
    return 2;
  }
  const char* xclbin = argv[1];
  int32_t dims[3] = {32, 32, soda::app::kStencilDim2 + 1};
  if (argc > 2) {
    dims[0] = std::max(dims[0], atoi(argv[2]));
  }
  if (argc > 3) {
    dims[1] = std::max(dims[1], atoi(argv[3]));
  }
  if (argc > 4) {
    dims[2] = std::max(dims[2], atoi(argv[4]));
  }

  vector<float> data_f(dims[0] * dims[1] * dims[2]);
  int32_t extent_f[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_f[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_f[3] = {0, 0, 0};
  vector<float> data_u(dims[0] * dims[1] * dims[2]);
  int32_t extent_u[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_u[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_u[3] = {0, 0, 0};
  vector<float> data_diff_u(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_u[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_u[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_u[3] = {0, 0, 0};
  vector<float> data_diff_d(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_d[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_d[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_d[3] = {0, 0, 0};
  vector<float> data_diff_l(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_l[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_l[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_l[3] = {0, 0, 0};
  vector<float> data_diff_r(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_r[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_r[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_r[3] = {0, 0, 0};
  vector<float> data_diff_i(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_i[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_i[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_i[3] = {0, 0, 0};
  vector<float> data_diff_o(dims[0] * dims[1] * dims[2]);
  int32_t extent_diff_o[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_diff_o[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_diff_o[3] = {0, 0, 0};
  vector<float> data_g(dims[0] * dims[1] * dims[2]);
  int32_t extent_g[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_g[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_g[3] = {0, 0, 0};
  vector<float> data_r0(dims[0] * dims[1] * dims[2]);
  int32_t extent_r0[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_r0[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_r0[3] = {0, 0, 0};
  vector<float> data_r1(dims[0] * dims[1] * dims[2]);
  int32_t extent_r1[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_r1[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_r1[3] = {0, 0, 0};
  vector<float> data_output(dims[0] * dims[1] * dims[2]);
  int32_t extent_output[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_output[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_output[3] = {0, 0, 0};

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);



  for(int32_t r = 0; r < dims[2]; ++r)
  {
    for(int32_t q = 0; q < dims[1]; ++q)
    {
      for(int32_t p = 0; p < dims[0]; ++p)
      {
        data_f[p * stride_f[0] + q * stride_f[1] + r * stride_f[2]] = distribution(generator);
      }
    }
  }



  for(int32_t r = 0; r < dims[2]; ++r)
  {
    for(int32_t q = 0; q < dims[1]; ++q)
    {
      for(int32_t p = 0; p < dims[0]; ++p)
      {
        data_u[p * stride_u[0] + q * stride_u[1] + r * stride_u[2]] = distribution(generator);
      }
    }
  }

  soda::app::denoise3d(
    data_f.data(),
    extent_f,
    stride_f,
    min_f,
    data_u.data(),
    extent_u,
    stride_u,
    min_u,
    data_output.data(),
    extent_output,
    stride_output,
    min_output);

  int error_count = 0;



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 1; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_diff_d[p*stride_diff_d[0]+q*stride_diff_d[1]+r*stride_diff_d[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p+0)*stride_u[0] + (q+1)*stride_u[1] + (r+0)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 1; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_diff_i[p*stride_diff_i[0]+q*stride_diff_i[1]+r*stride_diff_i[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r-1)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 1; p < dims[0] - 0; ++p)
      {

        data_diff_l[p*stride_diff_l[0]+q*stride_diff_l[1]+r*stride_diff_l[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p-1)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 1; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_diff_o[p*stride_diff_o[0]+q*stride_diff_o[1]+r*stride_diff_o[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+1)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 1; ++p)
      {

        data_diff_r[p*stride_diff_r[0]+q*stride_diff_r[1]+r*stride_diff_r[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p+1)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 1; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_diff_u[p*stride_diff_u[0]+q*stride_diff_u[1]+r*stride_diff_u[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] - data_u[(p+0)*stride_u[0] + (q-1)*stride_u[1] + (r+0)*stride_u[2]]);
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_r0[p*stride_r0[0]+q*stride_r0[1]+r*stride_r0[2]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] * data_f[(p+0)*stride_f[0] + (q+0)*stride_f[1] + (r+0)*stride_f[2]] * (1.0f / 0.03f));
      }
    }
  }



  for(int32_t r = 1; r < dims[2] - 1; ++r)
  {
    for(int32_t q = 1; q < dims[1] - 1; ++q)
    {
      for(int32_t p = 1; p < dims[0] - 1; ++p)
      {

        data_g[p*stride_g[0]+q*stride_g[1]+r*stride_g[2]] = (1.0f / sqrt((0.00005f + (data_diff_u[(p+0)*stride_diff_u[0] + (q+0)*stride_diff_u[1] + (r+0)*stride_diff_u[2]] * data_diff_u[(p+0)*stride_diff_u[0] + (q+0)*stride_diff_u[1] + (r+0)*stride_diff_u[2]]) + (data_diff_d[(p+0)*stride_diff_d[0] + (q+0)*stride_diff_d[1] + (r+0)*stride_diff_d[2]] * data_diff_d[(p+0)*stride_diff_d[0] + (q+0)*stride_diff_d[1] + (r+0)*stride_diff_d[2]]) + (data_diff_l[(p+0)*stride_diff_l[0] + (q+0)*stride_diff_l[1] + (r+0)*stride_diff_l[2]] * data_diff_l[(p+0)*stride_diff_l[0] + (q+0)*stride_diff_l[1] + (r+0)*stride_diff_l[2]]) + (data_diff_r[(p+0)*stride_diff_r[0] + (q+0)*stride_diff_r[1] + (r+0)*stride_diff_r[2]] * data_diff_r[(p+0)*stride_diff_r[0] + (q+0)*stride_diff_r[1] + (r+0)*stride_diff_r[2]]) + (data_diff_i[(p+0)*stride_diff_i[0] + (q+0)*stride_diff_i[1] + (r+0)*stride_diff_i[2]] * data_diff_i[(p+0)*stride_diff_i[0] + (q+0)*stride_diff_i[1] + (r+0)*stride_diff_i[2]]) + (data_diff_o[(p+0)*stride_diff_o[0] + (q+0)*stride_diff_o[1] + (r+0)*stride_diff_o[2]] * data_diff_o[(p+0)*stride_diff_o[0] + (q+0)*stride_diff_o[1] + (r+0)*stride_diff_o[2]]))));
      }
    }
  }



  for(int32_t r = 0; r < dims[2] - 0; ++r)
  {
    for(int32_t q = 0; q < dims[1] - 0; ++q)
    {
      for(int32_t p = 0; p < dims[0] - 0; ++p)
      {

        data_r1[p*stride_r1[0]+q*stride_r1[1]+r*stride_r1[2]] = ((data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]] * (2.38944f + (data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]] * (0.950037f + data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]])))) / (4.65314f + (data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]] * (2.57541f + (data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]] * (1.48937f + data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1] + (r+0)*stride_r0[2]]))))));
      }
    }
  }



  for(int32_t r = 2; r < dims[2] - 2; ++r)
  {
    for(int32_t q = 2; q < dims[1] - 2; ++q)
    {
      for(int32_t p = 2; p < dims[0] - 2; ++p)
      {

        float result_output = ((data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] + (5.0f * ((data_u[(p+1)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] * data_g[(p+1)*stride_g[0] + (q+0)*stride_g[1] + (r+0)*stride_g[2]]) + (data_u[(p-1)*stride_u[0] + (q+0)*stride_u[1] + (r+0)*stride_u[2]] * data_g[(p-1)*stride_g[0] + (q+0)*stride_g[1] + (r+0)*stride_g[2]]) + (data_u[(p+0)*stride_u[0] + (q+1)*stride_u[1] + (r+0)*stride_u[2]] * data_g[(p+0)*stride_g[0] + (q+1)*stride_g[1] + (r+0)*stride_g[2]]) + (data_u[(p+0)*stride_u[0] + (q-1)*stride_u[1] + (r+0)*stride_u[2]] * data_g[(p+0)*stride_g[0] + (q-1)*stride_g[1] + (r+0)*stride_g[2]]) + (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r+1)*stride_u[2]] * data_g[(p+0)*stride_g[0] + (q+0)*stride_g[1] + (r+1)*stride_g[2]]) + (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1] + (r-1)*stride_u[2]] * data_g[(p+0)*stride_g[0] + (q+0)*stride_g[1] + (r-1)*stride_g[2]]) + ((1.0f / 0.03f) * data_f[(p+0)*stride_f[0] + (q+0)*stride_f[1] + (r+0)*stride_f[2]] * data_r1[(p+0)*stride_r1[0] + (q+0)*stride_r1[1] + (r+0)*stride_r1[2]])))) / (1.0f + (5.0f * (data_g[(p+1)*stride_g[0] + (q+0)*stride_g[1] + (r+0)*stride_g[2]] + data_g[(p-1)*stride_g[0] + (q+0)*stride_g[1] + (r+0)*stride_g[2]] + data_g[(p+0)*stride_g[0] + (q+1)*stride_g[1] + (r+0)*stride_g[2]] + data_g[(p+0)*stride_g[0] + (q-1)*stride_g[1] + (r+0)*stride_g[2]] + data_g[(p+0)*stride_g[0] + (q+0)*stride_g[1] + (r+1)*stride_g[2]] + data_g[(p+0)*stride_g[0] + (q+0)*stride_g[1] + (r-1)*stride_g[2]] + (1.0f / 0.03f)))));
        float val_fpga = data_output[p * stride_output[0] + q * stride_output[1] + r * stride_output[2]];
        float val_cpu = result_output;
        double threshold = 0.00001;
        if (nullptr != getenv("THRESHOLD")) {
          threshold = atof(getenv("THRESHOLD"));
        }
        threshold *= threshold;
        if (double(val_fpga-val_cpu) * double(val_fpga-val_cpu) > threshold && double(val_fpga-val_cpu) * double(val_fpga-val_cpu) / (double(val_cpu) * double(val_cpu)) > threshold) {
          fprintf(stderr, "%lf != %lf @(%d, %d, %d)\n", double(val_fpga), double(val_cpu), p, q, r);
          ++error_count;
        }
      }
    }
  }

  clog << "INFO: " << (error_count ? "FAIL" : "PASS") << "!\n";

  return error_count ? 1 : 0;
}
