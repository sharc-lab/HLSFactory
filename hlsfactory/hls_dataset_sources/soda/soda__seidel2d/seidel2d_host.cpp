#line 18 "soda_designs_processed/soda__seidel2d/seidel2d_host.cpp"
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
void seidel2d_kernel(
  ap_uint<64>* bank_1_output,
  ap_uint<64>* bank_0_input,
  uint64_t coalesced_data_num);
}

namespace soda {
namespace app {

constexpr int kStencilDim0 = 5;
constexpr int kStencilDim1 = 5;
constexpr int kStencilDistance = 132;
constexpr int kWidth_input = 32;
constexpr int kWidth_output = 32;


using Type_input = float;
using Type_output = float;

int seidel2d(
  const Type_input* var_input_ptr,
  const int32_t var_input_extent[2],
  const int32_t var_input_stride[2],
  const int32_t var_input_min[2],
  Type_output* var_output_ptr,
  const int32_t var_output_extent[2],
  const int32_t var_output_stride[2],
  const int32_t var_output_min[2],
  // const char* bitstream,
  const int burst_width = 64,
  const int tile_size_0 = 32,
  const int unroll_factor = 2)
{


  int bank_count_input = 1;
  int bank_count_output = 1;
#line 103 "soda_designs_processed/soda__seidel2d/seidel2d_host.cpp"
  auto round_up = [](int64_t a, int64_t b) -> int64_t { return ((a - 1) / b + 1) * b; };


  const int elem_count_per_cycle_input = burst_width / kWidth_input * bank_count_input;
  const int elem_count_per_cycle_output = burst_width / kWidth_output * bank_count_output;
  int32_t tile_count_dim_0 = (var_input_extent[0] - kStencilDim0 + 1 - 1) / (tile_size_0 - kStencilDim0 + 1) + 1;
  int64_t tile_count = tile_count_dim_0;


  int64_t elem_count_per_tile = tile_size_0 * var_input_extent[1];
  int64_t cycle_count_per_tile = (elem_count_per_tile - 1) / elem_count_per_cycle_input + 1;
  int64_t elem_count_aligned_per_tile_i = cycle_count_per_tile * elem_count_per_cycle_input;
  int64_t elem_count_aligned_per_tile_o = cycle_count_per_tile * elem_count_per_cycle_output;


  int64_t buf_size_input = (tile_count * elem_count_aligned_per_tile_i + round_up(kStencilDistance, elem_count_per_cycle_input)) / bank_count_input * sizeof(Type_input);
  int64_t buf_size_output = (tile_count * elem_count_aligned_per_tile_o + round_up(kStencilDistance, elem_count_per_cycle_output)) / bank_count_output * sizeof(Type_output);


  vector<unique_ptr<Type_input, decltype(&free)>> buf_input;
  buf_input.reserve(bank_count_input);
  for (int bank = 0; bank < bank_count_input; ++bank) {
    buf_input.emplace_back(static_cast<Type_input*>(aligned_alloc(4096, round_up(buf_size_input, 4096))), &free);
  }
  vector<unique_ptr<Type_output, decltype(&free)>> buf_output;
  buf_output.reserve(bank_count_output);
  for (int bank = 0; bank < bank_count_output; ++bank) {
    buf_output.emplace_back(static_cast<Type_output*>(aligned_alloc(4096, round_up(buf_size_output, 4096))), &free);
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
  clog << "INFO: output.extent[0] = " << var_output_extent[0] << ", output.extent[1] = " << var_output_extent[1] << "" << endl;
  clog << "INFO: output.stride[0] = " << var_output_stride[0] << ", output.stride[1] = " << var_output_stride[1] << "" << endl;
  clog << "INFO: output.min[0] = " << var_output_min[0] << ", output.min[1] = " << var_output_min[1] << "" << endl;

  int64_t tile_data_count = ((int64_t(var_input_extent[1]) * tile_size_0 - 1) / elem_count_per_cycle_input + 1) * elem_count_per_cycle_input / unroll_factor;
  int64_t cycle_count = ((int64_t(var_input_extent[1]) * tile_size_0 * tile_count_dim_0 + kStencilDistance - 1) / elem_count_per_cycle_input + 1);
  clog << "INFO: tile_data_count = " << tile_data_count << ", cycle_count = " << cycle_count << endl;


  ::seidel2d_kernel(
    (ap_uint<64>*)buf_output[0].get(),
    (ap_uint<64>*)buf_input[0].get(),
    cycle_count);
#line 209 "soda_designs_processed/soda__seidel2d/seidel2d_host.cpp"
  for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
  {
    int32_t actual_tile_size_dim_0 = (tile_index_dim_0 == tile_count_dim_0-1) ? var_input_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

    for(int32_t j = 2; j < var_output_extent[1] - 2; ++j)
    {
      for(int32_t i = 2; i < actual_tile_size_dim_0 - 2; ++i)
      {



        int32_t p = tile_index_dim_0 * (tile_size_0-kStencilDim0 + 1) + i;
        int32_t q = j;
        int64_t original_offset = p * var_output_stride[0] + q * var_output_stride[1];
        int32_t burst_index_output = (i + j * tile_size_0 + 66) / elem_count_per_cycle_output;
        int32_t burst_residue_output = (i + j * tile_size_0 + 66) % elem_count_per_cycle_output;
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

  vector<float> data_input(dims[0] * dims[1]);
  int32_t extent_input[2] = {dims[0], dims[1]};
  int32_t stride_input[2] = {1, dims[0]};
  int32_t min_input[2] = {0, 0};
  vector<float> data_input_iter1(dims[0] * dims[1]);
  int32_t extent_input_iter1[2] = {dims[0], dims[1]};
  int32_t stride_input_iter1[2] = {1, dims[0]};
  int32_t min_input_iter1[2] = {0, 0};
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
      data_input[p * stride_input[0] + q * stride_input[1]] = distribution(generator);
    }
  }

  soda::app::seidel2d(
    data_input.data(),
    extent_input,
    stride_input,
    min_input,
    data_output.data(),
    extent_output,
    stride_output,
    min_output);

  int error_count = 0;



  for(int32_t q = 1; q < dims[1] - 1; ++q)
  {
    for(int32_t p = 1; p < dims[0] - 1; ++p)
    {

      data_input_iter1[p*stride_input_iter1[0]+q*stride_input_iter1[1]] = ((data_input[(p-1)*stride_input[0] + (q-1)*stride_input[1]] + data_input[(p-1)*stride_input[0] + (q+0)*stride_input[1]] + data_input[(p-1)*stride_input[0] + (q+1)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q-1)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+0)*stride_input[1]] + data_input[(p+0)*stride_input[0] + (q+1)*stride_input[1]] + data_input[(p+1)*stride_input[0] + (q-1)*stride_input[1]] + data_input[(p+1)*stride_input[0] + (q+0)*stride_input[1]] + data_input[(p+1)*stride_input[0] + (q+1)*stride_input[1]]) * .1111111f);
    }
  }



  for(int32_t q = 2; q < dims[1] - 2; ++q)
  {
    for(int32_t p = 2; p < dims[0] - 2; ++p)
    {

      float result_output = ((data_input_iter1[(p-1)*stride_input_iter1[0] + (q-1)*stride_input_iter1[1]] + data_input_iter1[(p-1)*stride_input_iter1[0] + (q+0)*stride_input_iter1[1]] + data_input_iter1[(p-1)*stride_input_iter1[0] + (q+1)*stride_input_iter1[1]] + data_input_iter1[(p+0)*stride_input_iter1[0] + (q-1)*stride_input_iter1[1]] + data_input_iter1[(p+0)*stride_input_iter1[0] + (q+0)*stride_input_iter1[1]] + data_input_iter1[(p+0)*stride_input_iter1[0] + (q+1)*stride_input_iter1[1]] + data_input_iter1[(p+1)*stride_input_iter1[0] + (q-1)*stride_input_iter1[1]] + data_input_iter1[(p+1)*stride_input_iter1[0] + (q+0)*stride_input_iter1[1]] + data_input_iter1[(p+1)*stride_input_iter1[0] + (q+1)*stride_input_iter1[1]]) * .1111111f);
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
