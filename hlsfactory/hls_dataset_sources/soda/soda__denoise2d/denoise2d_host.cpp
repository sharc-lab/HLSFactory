#line 26 "soda_designs_processed/soda__denoise2d/denoise2d_host.cpp"
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
void denoise2d_kernel(
  ap_uint<128>* bank_2_output,
  ap_uint<128>* bank_0_f,
  ap_uint<128>* bank_1_u,
  uint64_t coalesced_data_num);
}

namespace soda {
namespace app {

constexpr int kStencilDim0 = 5;
constexpr int kStencilDim1 = 5;
constexpr int kStencilDistance = 130;
constexpr int kWidth_f = 32;
constexpr int kWidth_u = 32;
constexpr int kWidth_output = 32;


using Type_f = float;
using Type_u = float;
using Type_output = float;

int denoise2d(
  const Type_f* var_f_ptr,
  const int32_t var_f_extent[2],
  const int32_t var_f_stride[2],
  const int32_t var_f_min[2],
  const Type_u* var_u_ptr,
  const int32_t var_u_extent[2],
  const int32_t var_u_stride[2],
  const int32_t var_u_min[2],
  Type_output* var_output_ptr,
  const int32_t var_output_extent[2],
  const int32_t var_output_stride[2],
  const int32_t var_output_min[2],
  // const char* bitstream,
  const int burst_width = 128,
  const int tile_size_0 = 32,
  const int unroll_factor = 4)
{


  int bank_count_f = 1;
  int bank_count_u = 1;
  int bank_count_output = 1;
#line 122 "soda_designs_processed/soda__denoise2d/denoise2d_host.cpp"
  auto round_up = [](int64_t a, int64_t b) -> int64_t { return ((a - 1) / b + 1) * b; };


  const int elem_count_per_cycle_f = burst_width / kWidth_f * bank_count_f;
  const int elem_count_per_cycle_u = burst_width / kWidth_u * bank_count_u;
  const int elem_count_per_cycle_output = burst_width / kWidth_output * bank_count_output;
  int32_t tile_count_dim_0 = (var_f_extent[0] - kStencilDim0 + 1 - 1) / (tile_size_0 - kStencilDim0 + 1) + 1;
  int64_t tile_count = tile_count_dim_0;


  int64_t elem_count_per_tile = tile_size_0 * var_f_extent[1];
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


  for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
  {
    int32_t actual_tile_size_dim_0 = (tile_index_dim_0==tile_count_dim_0-1) ? var_f_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

    for(int32_t j = 0; j < var_f_extent[1]; ++j)
    {
      for(int32_t i = 0; i < actual_tile_size_dim_0; ++i)
      {



        int32_t burst_index = (i + j * tile_size_0) / elem_count_per_cycle_f;
        int32_t burst_residue = (i + j * tile_size_0) % elem_count_per_cycle_f;
        int32_t p = tile_index_dim_0 * (tile_size_0 - kStencilDim0) + i;
        int32_t q = j;
        int64_t tiled_offset = (tile_index_dim_0) * elem_count_aligned_per_tile_i + burst_index * elem_count_per_cycle_f + burst_residue;
        int64_t original_offset = p * var_f_stride[0] + q * var_f_stride[1];
        buf_f[tiled_offset % bank_count_f].get()[tiled_offset / bank_count_f] = var_f_ptr[std::max(int64_t(0), original_offset - 64)];
        buf_u[tiled_offset % bank_count_u].get()[tiled_offset / bank_count_u] = var_u_ptr[std::max(int64_t(0), original_offset - 0)];
      }
    }
  }

  clog << "INFO: tile_count[0] = " << tile_count_dim_0 << ", tile_size[0] = " << tile_size_0 << endl;
  clog << "INFO: f.extent[0] = " << var_f_extent[0] << ", f.extent[1] = " << var_f_extent[1] << "" << endl;
  clog << "INFO: f.stride[0] = " << var_f_stride[0] << ", f.stride[1] = " << var_f_stride[1] << "" << endl;
  clog << "INFO: f.min[0] = " << var_f_min[0] << ", f.min[1] = " << var_f_min[1] << "" << endl;
  clog << "INFO: u.extent[0] = " << var_u_extent[0] << ", u.extent[1] = " << var_u_extent[1] << "" << endl;
  clog << "INFO: u.stride[0] = " << var_u_stride[0] << ", u.stride[1] = " << var_u_stride[1] << "" << endl;
  clog << "INFO: u.min[0] = " << var_u_min[0] << ", u.min[1] = " << var_u_min[1] << "" << endl;
  clog << "INFO: output.extent[0] = " << var_output_extent[0] << ", output.extent[1] = " << var_output_extent[1] << "" << endl;
  clog << "INFO: output.stride[0] = " << var_output_stride[0] << ", output.stride[1] = " << var_output_stride[1] << "" << endl;
  clog << "INFO: output.min[0] = " << var_output_min[0] << ", output.min[1] = " << var_output_min[1] << "" << endl;

  int64_t tile_data_count = ((int64_t(var_f_extent[1]) * tile_size_0 - 1) / elem_count_per_cycle_f + 1) * elem_count_per_cycle_f / unroll_factor;
  int64_t cycle_count = ((int64_t(var_f_extent[1]) * tile_size_0 * tile_count_dim_0 + kStencilDistance - 1) / elem_count_per_cycle_f + 1);
  clog << "INFO: tile_data_count = " << tile_data_count << ", cycle_count = " << cycle_count << endl;


  ::denoise2d_kernel(
    (ap_uint<128>*)buf_output[0].get(),
    (ap_uint<128>*)buf_f[0].get(),
    (ap_uint<128>*)buf_u[0].get(),
    cycle_count);
#line 248 "soda_designs_processed/soda__denoise2d/denoise2d_host.cpp"
  for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
  {
    int32_t actual_tile_size_dim_0 = (tile_index_dim_0 == tile_count_dim_0-1) ? var_f_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

    for(int32_t j = 0; j < var_output_extent[1] - 0; ++j)
    {
      for(int32_t i = 0; i < actual_tile_size_dim_0 - 0; ++i)
      {



        int32_t p = tile_index_dim_0 * (tile_size_0-kStencilDim0 + 1) + i;
        int32_t q = j;
        int64_t original_offset = p * var_output_stride[0] + q * var_output_stride[1];
        int32_t burst_index_output = (i + j * tile_size_0 + 64) / elem_count_per_cycle_output;
        int32_t burst_residue_output = (i + j * tile_size_0 + 64) % elem_count_per_cycle_output;
        int64_t tiled_offset_output = (tile_index_dim_0) * elem_count_aligned_per_tile_o + burst_index_output * elem_count_per_cycle_output + burst_residue_output;
        var_output_ptr[original_offset] = buf_output[tiled_offset_output % bank_count_output].get()[tiled_offset_output / bank_count_output];
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

  vector<float> data_f(dims[0] * dims[1]);
  int32_t extent_f[2] = {dims[0], dims[1]};
  int32_t stride_f[2] = {1, dims[0]};
  int32_t min_f[2] = {0, 0};
  vector<float> data_u(dims[0] * dims[1]);
  int32_t extent_u[2] = {dims[0], dims[1]};
  int32_t stride_u[2] = {1, dims[0]};
  int32_t min_u[2] = {0, 0};
  vector<float> data_diff_u(dims[0] * dims[1]);
  int32_t extent_diff_u[2] = {dims[0], dims[1]};
  int32_t stride_diff_u[2] = {1, dims[0]};
  int32_t min_diff_u[2] = {0, 0};
  vector<float> data_diff_d(dims[0] * dims[1]);
  int32_t extent_diff_d[2] = {dims[0], dims[1]};
  int32_t stride_diff_d[2] = {1, dims[0]};
  int32_t min_diff_d[2] = {0, 0};
  vector<float> data_diff_l(dims[0] * dims[1]);
  int32_t extent_diff_l[2] = {dims[0], dims[1]};
  int32_t stride_diff_l[2] = {1, dims[0]};
  int32_t min_diff_l[2] = {0, 0};
  vector<float> data_diff_r(dims[0] * dims[1]);
  int32_t extent_diff_r[2] = {dims[0], dims[1]};
  int32_t stride_diff_r[2] = {1, dims[0]};
  int32_t min_diff_r[2] = {0, 0};
  vector<float> data_g(dims[0] * dims[1]);
  int32_t extent_g[2] = {dims[0], dims[1]};
  int32_t stride_g[2] = {1, dims[0]};
  int32_t min_g[2] = {0, 0};
  vector<float> data_r0(dims[0] * dims[1]);
  int32_t extent_r0[2] = {dims[0], dims[1]};
  int32_t stride_r0[2] = {1, dims[0]};
  int32_t min_r0[2] = {0, 0};
  vector<float> data_r1(dims[0] * dims[1]);
  int32_t extent_r1[2] = {dims[0], dims[1]};
  int32_t stride_r1[2] = {1, dims[0]};
  int32_t min_r1[2] = {0, 0};
  vector<float> data_output(dims[0] * dims[1]);
  int32_t extent_output[2] = {dims[0], dims[1]};
  int32_t stride_output[2] = {1, dims[0]};
  int32_t min_output[2] = {0, 0};

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);



  for(int32_t q = 0; q < dims[1]; ++q)
  {
    for(int32_t p = 0; p < dims[0]; ++p)
    {
      data_f[p * stride_f[0] + q * stride_f[1]] = distribution(generator);
    }
  }



  for(int32_t q = 0; q < dims[1]; ++q)
  {
    for(int32_t p = 0; p < dims[0]; ++p)
    {
      data_u[p * stride_u[0] + q * stride_u[1]] = distribution(generator);
    }
  }

  soda::app::denoise2d(
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



  for(int32_t q = 0; q < dims[1] - 1; ++q)
  {
    for(int32_t p = 0; p < dims[0] - 0; ++p)
    {

      data_diff_d[p*stride_diff_d[0]+q*stride_diff_d[1]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1]] - data_u[(p+0)*stride_u[0] + (q+1)*stride_u[1]]);
    }
  }



  for(int32_t q = 0; q < dims[1] - 0; ++q)
  {
    for(int32_t p = 1; p < dims[0] - 0; ++p)
    {

      data_diff_l[p*stride_diff_l[0]+q*stride_diff_l[1]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1]] - data_u[(p-1)*stride_u[0] + (q+0)*stride_u[1]]);
    }
  }



  for(int32_t q = 0; q < dims[1] - 0; ++q)
  {
    for(int32_t p = 0; p < dims[0] - 1; ++p)
    {

      data_diff_r[p*stride_diff_r[0]+q*stride_diff_r[1]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1]] - data_u[(p+1)*stride_u[0] + (q+0)*stride_u[1]]);
    }
  }



  for(int32_t q = 1; q < dims[1] - 0; ++q)
  {
    for(int32_t p = 0; p < dims[0] - 0; ++p)
    {

      data_diff_u[p*stride_diff_u[0]+q*stride_diff_u[1]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1]] - data_u[(p+0)*stride_u[0] + (q-1)*stride_u[1]]);
    }
  }



  for(int32_t q = 0; q < dims[1] - 0; ++q)
  {
    for(int32_t p = 0; p < dims[0] - 0; ++p)
    {

      data_r0[p*stride_r0[0]+q*stride_r0[1]] = (data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1]] * data_f[(p+0)*stride_f[0] + (q+0)*stride_f[1]] * 4.9f);
    }
  }



  for(int32_t q = 1; q < dims[1] - 1; ++q)
  {
    for(int32_t p = 1; p < dims[0] - 1; ++p)
    {

      data_g[p*stride_g[0]+q*stride_g[1]] = (1.0f / sqrt((1.0f + (data_diff_u[(p+0)*stride_diff_u[0] + (q+0)*stride_diff_u[1]] * data_diff_u[(p+0)*stride_diff_u[0] + (q+0)*stride_diff_u[1]]) + (data_diff_d[(p+0)*stride_diff_d[0] + (q+0)*stride_diff_d[1]] * data_diff_d[(p+0)*stride_diff_d[0] + (q+0)*stride_diff_d[1]]) + (data_diff_l[(p+0)*stride_diff_l[0] + (q+0)*stride_diff_l[1]] * data_diff_l[(p+0)*stride_diff_l[0] + (q+0)*stride_diff_l[1]]) + (data_diff_r[(p+0)*stride_diff_r[0] + (q+0)*stride_diff_r[1]] * data_diff_r[(p+0)*stride_diff_r[0] + (q+0)*stride_diff_r[1]]))));
    }
  }



  for(int32_t q = 0; q < dims[1] - 0; ++q)
  {
    for(int32_t p = 0; p < dims[0] - 0; ++p)
    {

      data_r1[p*stride_r1[0]+q*stride_r1[1]] = ((data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1]] * (2.5f + (data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1]] * (10.2f + data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1]])))) * (4.3f + (data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1]] * (5.4f + (data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1]] * (6.3f + data_r0[(p+0)*stride_r0[0] + (q+0)*stride_r0[1]]))))));
    }
  }



  for(int32_t q = 2; q < dims[1] - 2; ++q)
  {
    for(int32_t p = 2; p < dims[0] - 2; ++p)
    {

      float result_output = ((data_u[(p+0)*stride_u[0] + (q+0)*stride_u[1]] + (7.7f * ((data_u[(p+0)*stride_u[0] + (q+1)*stride_u[1]] * data_g[(p+0)*stride_g[0] + (q+1)*stride_g[1]]) + (data_u[(p+0)*stride_u[0] + (q-1)*stride_u[1]] * data_g[(p+0)*stride_g[0] + (q-1)*stride_g[1]]) + (data_u[(p-1)*stride_u[0] + (q+0)*stride_u[1]] * data_g[(p-1)*stride_g[0] + (q+0)*stride_g[1]]) + (data_u[(p+1)*stride_u[0] + (q+0)*stride_u[1]] * data_g[(p+1)*stride_g[0] + (q+0)*stride_g[1]]) + (5.7f * data_f[(p+0)*stride_f[0] + (q+0)*stride_f[1]] * data_r1[(p+0)*stride_r1[0] + (q+0)*stride_r1[1]])))) * (11.1f + (7.7f * (data_g[(p+0)*stride_g[0] + (q+1)*stride_g[1]] + data_g[(p+0)*stride_g[0] + (q-1)*stride_g[1]] + data_g[(p-1)*stride_g[0] + (q+0)*stride_g[1]] + data_g[(p+1)*stride_g[0] + (q+0)*stride_g[1]] + 5.7f))));
      float val_fpga = data_output[p * stride_output[0] + q * stride_output[1]];
      float val_cpu = result_output;
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
