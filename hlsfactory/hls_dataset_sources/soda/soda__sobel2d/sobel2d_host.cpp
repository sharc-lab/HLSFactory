#line 20 "soda_designs_processed/soda__sobel2d/sobel2d_host.cpp"
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
void sobel2d_kernel(
  ap_uint<64>* bank_1_mag,
  ap_uint<64>* bank_0_img,
  uint64_t coalesced_data_num);
}

namespace soda {
namespace app {

constexpr int kStencilDim0 = 3;
constexpr int kStencilDim1 = 3;
constexpr int kStencilDistance = 66;
constexpr int kWidth_img = 16;
constexpr int kWidth_mag = 16;


using Type_img = int16_t;
using Type_mag = uint16_t;

int sobel2d(
  const Type_img* var_img_ptr,
  const int32_t var_img_extent[2],
  const int32_t var_img_stride[2],
  const int32_t var_img_min[2],
  Type_mag* var_mag_ptr,
  const int32_t var_mag_extent[2],
  const int32_t var_mag_stride[2],
  const int32_t var_mag_min[2],
  // const char* bitstream,
  const int burst_width = 64,
  const int tile_size_0 = 32,
  const int unroll_factor = 4)
{


  int bank_count_img = 1;
  int bank_count_mag = 1;
#line 105 "soda_designs_processed/soda__sobel2d/sobel2d_host.cpp"
  auto round_up = [](int64_t a, int64_t b) -> int64_t { return ((a - 1) / b + 1) * b; };


  const int elem_count_per_cycle_img = burst_width / kWidth_img * bank_count_img;
  const int elem_count_per_cycle_mag = burst_width / kWidth_mag * bank_count_mag;
  int32_t tile_count_dim_0 = (var_img_extent[0] - kStencilDim0 + 1 - 1) / (tile_size_0 - kStencilDim0 + 1) + 1;
  int64_t tile_count = tile_count_dim_0;


  int64_t elem_count_per_tile = tile_size_0 * var_img_extent[1];
  int64_t cycle_count_per_tile = (elem_count_per_tile - 1) / elem_count_per_cycle_img + 1;
  int64_t elem_count_aligned_per_tile_i = cycle_count_per_tile * elem_count_per_cycle_img;
  int64_t elem_count_aligned_per_tile_o = cycle_count_per_tile * elem_count_per_cycle_mag;


  int64_t buf_size_img = (tile_count * elem_count_aligned_per_tile_i + round_up(kStencilDistance, elem_count_per_cycle_img)) / bank_count_img * sizeof(Type_img);
  int64_t buf_size_mag = (tile_count * elem_count_aligned_per_tile_o + round_up(kStencilDistance, elem_count_per_cycle_mag)) / bank_count_mag * sizeof(Type_mag);


  vector<unique_ptr<Type_img, decltype(&free)>> buf_img;
  buf_img.reserve(bank_count_img);
  for (int bank = 0; bank < bank_count_img; ++bank) {
    buf_img.emplace_back(static_cast<Type_img*>(aligned_alloc(4096, round_up(buf_size_img, 4096))), &free);
  }
  vector<unique_ptr<Type_mag, decltype(&free)>> buf_mag;
  buf_mag.reserve(bank_count_mag);
  for (int bank = 0; bank < bank_count_mag; ++bank) {
    buf_mag.emplace_back(static_cast<Type_mag*>(aligned_alloc(4096, round_up(buf_size_mag, 4096))), &free);
  }


  for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
  {
    int32_t actual_tile_size_dim_0 = (tile_index_dim_0==tile_count_dim_0-1) ? var_img_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

    for(int32_t j = 0; j < var_img_extent[1]; ++j)
    {
      for(int32_t i = 0; i < actual_tile_size_dim_0; ++i)
      {



        int32_t burst_index = (i + j * tile_size_0) / elem_count_per_cycle_img;
        int32_t burst_residue = (i + j * tile_size_0) % elem_count_per_cycle_img;
        int32_t p = tile_index_dim_0 * (tile_size_0 - kStencilDim0) + i;
        int32_t q = j;
        int64_t tiled_offset = (tile_index_dim_0) * elem_count_aligned_per_tile_i + burst_index * elem_count_per_cycle_img + burst_residue;
        int64_t original_offset = p * var_img_stride[0] + q * var_img_stride[1];
        buf_img[tiled_offset % bank_count_img].get()[tiled_offset / bank_count_img] = var_img_ptr[std::max(int64_t(0), original_offset - 0)];
      }
    }
  }

  clog << "INFO: tile_count[0] = " << tile_count_dim_0 << ", tile_size[0] = " << tile_size_0 << endl;
  clog << "INFO: img.extent[0] = " << var_img_extent[0] << ", img.extent[1] = " << var_img_extent[1] << "" << endl;
  clog << "INFO: img.stride[0] = " << var_img_stride[0] << ", img.stride[1] = " << var_img_stride[1] << "" << endl;
  clog << "INFO: img.min[0] = " << var_img_min[0] << ", img.min[1] = " << var_img_min[1] << "" << endl;
  clog << "INFO: mag.extent[0] = " << var_mag_extent[0] << ", mag.extent[1] = " << var_mag_extent[1] << "" << endl;
  clog << "INFO: mag.stride[0] = " << var_mag_stride[0] << ", mag.stride[1] = " << var_mag_stride[1] << "" << endl;
  clog << "INFO: mag.min[0] = " << var_mag_min[0] << ", mag.min[1] = " << var_mag_min[1] << "" << endl;

  int64_t tile_data_count = ((int64_t(var_img_extent[1]) * tile_size_0 - 1) / elem_count_per_cycle_img + 1) * elem_count_per_cycle_img / unroll_factor;
  int64_t cycle_count = ((int64_t(var_img_extent[1]) * tile_size_0 * tile_count_dim_0 + kStencilDistance - 1) / elem_count_per_cycle_img + 1);
  clog << "INFO: tile_data_count = " << tile_data_count << ", cycle_count = " << cycle_count << endl;


  ::sobel2d_kernel(
    (ap_uint<64>*)buf_mag[0].get(),
    (ap_uint<64>*)buf_img[0].get(),
    cycle_count);
#line 211 "soda_designs_processed/soda__sobel2d/sobel2d_host.cpp"
  for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
  {
    int32_t actual_tile_size_dim_0 = (tile_index_dim_0 == tile_count_dim_0-1) ? var_img_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

    for(int32_t j = 1; j < var_mag_extent[1] - 1; ++j)
    {
      for(int32_t i = 1; i < actual_tile_size_dim_0 - 1; ++i)
      {



        int32_t p = tile_index_dim_0 * (tile_size_0-kStencilDim0 + 1) + i;
        int32_t q = j;
        int64_t original_offset = p * var_mag_stride[0] + q * var_mag_stride[1];
        int32_t burst_index_mag = (i + j * tile_size_0 + 33) / elem_count_per_cycle_mag;
        int32_t burst_residue_mag = (i + j * tile_size_0 + 33) % elem_count_per_cycle_mag;
        int64_t tiled_offset_mag = (tile_index_dim_0) * elem_count_aligned_per_tile_o + burst_index_mag * elem_count_per_cycle_mag + burst_residue_mag;
        var_mag_ptr[original_offset] = buf_mag[tiled_offset_mag % bank_count_mag].get()[tiled_offset_mag / bank_count_mag];
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

  vector<int16_t> data_img(dims[0] * dims[1]);
  int32_t extent_img[2] = {dims[0], dims[1]};
  int32_t stride_img[2] = {1, dims[0]};
  int32_t min_img[2] = {0, 0};
  vector<int16_t> data_mag_x(dims[0] * dims[1]);
  int32_t extent_mag_x[2] = {dims[0], dims[1]};
  int32_t stride_mag_x[2] = {1, dims[0]};
  int32_t min_mag_x[2] = {0, 0};
  vector<uint16_t> data_mag_y(dims[0] * dims[1]);
  int32_t extent_mag_y[2] = {dims[0], dims[1]};
  int32_t stride_mag_y[2] = {1, dims[0]};
  int32_t min_mag_y[2] = {0, 0};
  vector<uint16_t> data_mag(dims[0] * dims[1]);
  int32_t extent_mag[2] = {dims[0], dims[1]};
  int32_t stride_mag[2] = {1, dims[0]};
  int32_t min_mag[2] = {0, 0};



  for(int32_t q = 0; q < dims[1]; ++q)
  {
    for(int32_t p = 0; p < dims[0]; ++p)
    {
      data_img[p * stride_img[0] + q * stride_img[1]] = p+q;
    }
  }

  soda::app::sobel2d(
    data_img.data(),
    extent_img,
    stride_img,
    min_img,
    data_mag.data(),
    extent_mag,
    stride_mag,
    min_mag);

  int error_count = 0;



  for(int32_t q = 1; q < dims[1] - 1; ++q)
  {
    for(int32_t p = 1; p < dims[0] - 1; ++p)
    {

      data_mag_x[p*stride_mag_x[0]+q*stride_mag_x[1]] = ((data_img[(p+1)*stride_img[0] + (q-1)*stride_img[1]] - data_img[(p-1)*stride_img[0] + (q-1)*stride_img[1]]) + ((data_img[(p+1)*stride_img[0] + (q+0)*stride_img[1]] - data_img[(p-1)*stride_img[0] + (q+0)*stride_img[1]]) * 3) + (data_img[(p+1)*stride_img[0] + (q+1)*stride_img[1]] - data_img[(p-1)*stride_img[0] + (q+1)*stride_img[1]]));
    }
  }



  for(int32_t q = 1; q < dims[1] - 1; ++q)
  {
    for(int32_t p = 1; p < dims[0] - 1; ++p)
    {

      data_mag_y[p*stride_mag_y[0]+q*stride_mag_y[1]] = ((data_img[(p-1)*stride_img[0] + (q+1)*stride_img[1]] - data_img[(p-1)*stride_img[0] + (q-1)*stride_img[1]]) + ((data_img[(p+0)*stride_img[0] + (q+1)*stride_img[1]] - data_img[(p+0)*stride_img[0] + (q-1)*stride_img[1]]) * 3) + (data_img[(p+1)*stride_img[0] + (q+1)*stride_img[1]] - data_img[(p+1)*stride_img[0] + (q-1)*stride_img[1]]));
    }
  }



  for(int32_t q = 1; q < dims[1] - 1; ++q)
  {
    for(int32_t p = 1; p < dims[0] - 1; ++p)
    {

      uint16_t result_mag = (65535 - ((data_mag_x[(p+0)*stride_mag_x[0] + (q+0)*stride_mag_x[1]] * data_mag_x[(p+0)*stride_mag_x[0] + (q+0)*stride_mag_x[1]]) + (data_mag_y[(p+0)*stride_mag_y[0] + (q+0)*stride_mag_y[1]] * data_mag_y[(p+0)*stride_mag_y[0] + (q+0)*stride_mag_y[1]])));
      uint16_t val_fpga = data_mag[p * stride_mag[0] + q * stride_mag[1]];
      uint16_t val_cpu = result_mag;
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
