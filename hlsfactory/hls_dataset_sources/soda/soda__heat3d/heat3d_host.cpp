#line 18 "soda_designs_processed/soda__heat3d/heat3d_host.cpp"
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
void heat3d_kernel(
  ap_uint<64>* bank_1_out,
  ap_uint<64>* bank_0_in,
  uint64_t coalesced_data_num);
}

namespace soda {
namespace app {

constexpr int kStencilDim0 = 5;
constexpr int kStencilDim1 = 5;
constexpr int kStencilDim2 = 5;
constexpr int kStencilDistance = 4162;
constexpr int kWidth_in = 32;
constexpr int kWidth_out = 32;


using Type_in = float;
using Type_out = float;

int heat3d(
  const Type_in* var_in_ptr,
  const int32_t var_in_extent[3],
  const int32_t var_in_stride[3],
  const int32_t var_in_min[3],
  Type_out* var_out_ptr,
  const int32_t var_out_extent[3],
  const int32_t var_out_stride[3],
  const int32_t var_out_min[3],
  // const char* bitstream,
  const int burst_width = 64,
  const int tile_size_0 = 32,
  const int tile_size_1 = 32,
  const int unroll_factor = 2)
{


  int bank_count_in = 1;
  int bank_count_out = 1;
#line 105 "soda_designs_processed/soda__heat3d/heat3d_host.cpp"
  auto round_up = [](int64_t a, int64_t b) -> int64_t { return ((a - 1) / b + 1) * b; };


  const int elem_count_per_cycle_in = burst_width / kWidth_in * bank_count_in;
  const int elem_count_per_cycle_out = burst_width / kWidth_out * bank_count_out;
  int32_t tile_count_dim_0 = (var_in_extent[0] - kStencilDim0 + 1 - 1) / (tile_size_0 - kStencilDim0 + 1) + 1;
  int32_t tile_count_dim_1 = (var_in_extent[1] - kStencilDim1 + 1 - 1) / (tile_size_1 - kStencilDim1 + 1) + 1;
  int64_t tile_count = tile_count_dim_0 * tile_count_dim_1;


  int64_t elem_count_per_tile = tile_size_0 * tile_size_1 * var_in_extent[2];
  int64_t cycle_count_per_tile = (elem_count_per_tile - 1) / elem_count_per_cycle_in + 1;
  int64_t elem_count_aligned_per_tile_i = cycle_count_per_tile * elem_count_per_cycle_in;
  int64_t elem_count_aligned_per_tile_o = cycle_count_per_tile * elem_count_per_cycle_out;


  int64_t buf_size_in = (tile_count * elem_count_aligned_per_tile_i + round_up(kStencilDistance, elem_count_per_cycle_in)) / bank_count_in * sizeof(Type_in);
  int64_t buf_size_out = (tile_count * elem_count_aligned_per_tile_o + round_up(kStencilDistance, elem_count_per_cycle_out)) / bank_count_out * sizeof(Type_out);


  vector<unique_ptr<Type_in, decltype(&free)>> buf_in;
  buf_in.reserve(bank_count_in);
  for (int bank = 0; bank < bank_count_in; ++bank) {
    buf_in.emplace_back(static_cast<Type_in*>(aligned_alloc(4096, round_up(buf_size_in, 4096))), &free);
  }
  vector<unique_ptr<Type_out, decltype(&free)>> buf_out;
  buf_out.reserve(bank_count_out);
  for (int bank = 0; bank < bank_count_out; ++bank) {
    buf_out.emplace_back(static_cast<Type_out*>(aligned_alloc(4096, round_up(buf_size_out, 4096))), &free);
  }


  for(int32_t tile_index_dim_1 = 0; tile_index_dim_1 < tile_count_dim_1; ++tile_index_dim_1)
  {
    int32_t actual_tile_size_dim_1 = (tile_index_dim_1==tile_count_dim_1-1) ? var_in_extent[1] - (tile_size_1 - kStencilDim1 + 1) * tile_index_dim_1 : tile_size_1;
    for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
    {
      int32_t actual_tile_size_dim_0 = (tile_index_dim_0==tile_count_dim_0-1) ? var_in_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

      for(int32_t k = 0; k < var_in_extent[2]; ++k)
      {
        for(int32_t j = 0; j < actual_tile_size_dim_1; ++j)
        {
          for(int32_t i = 0; i < actual_tile_size_dim_0; ++i)
          {



            int32_t burst_index = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1) / elem_count_per_cycle_in;
            int32_t burst_residue = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1) % elem_count_per_cycle_in;
            int32_t p = tile_index_dim_0 * (tile_size_0 - kStencilDim0) + i;
            int32_t q = tile_index_dim_1 * (tile_size_1 - kStencilDim1) + j;
            int32_t r = k;
            int64_t tiled_offset = (tile_index_dim_0 + tile_count_dim_0 * tile_index_dim_1) * elem_count_aligned_per_tile_i + burst_index * elem_count_per_cycle_in + burst_residue;
            int64_t original_offset = p * var_in_stride[0] + q * var_in_stride[1] + r * var_in_stride[2];
            buf_in[tiled_offset % bank_count_in].get()[tiled_offset / bank_count_in] = var_in_ptr[std::max(int64_t(0), original_offset - 0)];
          }
        }
      }
    }
  }

  clog << "INFO: tile_count[0] = " << tile_count_dim_0 << ", tile_size[0] = " << tile_size_0 << endl;
  clog << "INFO: tile_count[1] = " << tile_count_dim_1 << ", tile_size[1] = " << tile_size_1 << endl;
  clog << "INFO: in.extent[0] = " << var_in_extent[0] << ", in.extent[1] = " << var_in_extent[1] << ", in.extent[2] = " << var_in_extent[2] << "" << endl;
  clog << "INFO: in.stride[0] = " << var_in_stride[0] << ", in.stride[1] = " << var_in_stride[1] << ", in.stride[2] = " << var_in_stride[2] << "" << endl;
  clog << "INFO: in.min[0] = " << var_in_min[0] << ", in.min[1] = " << var_in_min[1] << ", in.min[2] = " << var_in_min[2] << "" << endl;
  clog << "INFO: out.extent[0] = " << var_out_extent[0] << ", out.extent[1] = " << var_out_extent[1] << ", out.extent[2] = " << var_out_extent[2] << "" << endl;
  clog << "INFO: out.stride[0] = " << var_out_stride[0] << ", out.stride[1] = " << var_out_stride[1] << ", out.stride[2] = " << var_out_stride[2] << "" << endl;
  clog << "INFO: out.min[0] = " << var_out_min[0] << ", out.min[1] = " << var_out_min[1] << ", out.min[2] = " << var_out_min[2] << "" << endl;

  int64_t tile_data_count = ((int64_t(var_in_extent[2]) * tile_size_0 * tile_size_1 - 1) / elem_count_per_cycle_in + 1) * elem_count_per_cycle_in / unroll_factor;
  int64_t cycle_count = ((int64_t(var_in_extent[2]) * tile_size_0 * tile_size_1 * tile_count_dim_0 * tile_count_dim_1 + kStencilDistance - 1) / elem_count_per_cycle_in + 1);
  clog << "INFO: tile_data_count = " << tile_data_count << ", cycle_count = " << cycle_count << endl;


  ::heat3d_kernel(
    (ap_uint<64>*)buf_out[0].get(),
    (ap_uint<64>*)buf_in[0].get(),
    cycle_count);
#line 221 "soda_designs_processed/soda__heat3d/heat3d_host.cpp"
  for(int32_t tile_index_dim_1 = 0; tile_index_dim_1 < tile_count_dim_1; ++tile_index_dim_1)
  {
    int32_t actual_tile_size_dim_1 = (tile_index_dim_1 == tile_count_dim_1-1) ? var_in_extent[1] - (tile_size_1 - kStencilDim1 + 1) * tile_index_dim_1 : tile_size_1;
    for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
    {
      int32_t actual_tile_size_dim_0 = (tile_index_dim_0 == tile_count_dim_0-1) ? var_in_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

      for(int32_t k = 2; k < var_out_extent[2] - 2; ++k)
      {
        for(int32_t j = 2; j < actual_tile_size_dim_1 - 2; ++j)
        {
          for(int32_t i = 2; i < actual_tile_size_dim_0 - 2; ++i)
          {



            int32_t p = tile_index_dim_0 * (tile_size_0-kStencilDim0 + 1) + i;
            int32_t q = tile_index_dim_1 * (tile_size_1-kStencilDim1 + 1) + j;
            int32_t r = k;
            int64_t original_offset = p * var_out_stride[0] + q * var_out_stride[1] + r * var_out_stride[2];
            int32_t burst_index_out = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1 + 2048) / elem_count_per_cycle_out;
            int32_t burst_residue_out = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1 + 2048) % elem_count_per_cycle_out;
            int64_t tiled_offset_out = (tile_index_dim_0+tile_count_dim_0 * tile_index_dim_1) * elem_count_aligned_per_tile_o + burst_index_out * elem_count_per_cycle_out + burst_residue_out;
            var_out_ptr[original_offset] = buf_out[tiled_offset_out % bank_count_out].get()[tiled_offset_out / bank_count_out];
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

  vector<float> data_in(dims[0] * dims[1] * dims[2]);
  int32_t extent_in[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_in[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_in[3] = {0, 0, 0};
  vector<float> data_in_iter1(dims[0] * dims[1] * dims[2]);
  int32_t extent_in_iter1[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_in_iter1[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_in_iter1[3] = {0, 0, 0};
  vector<float> data_out(dims[0] * dims[1] * dims[2]);
  int32_t extent_out[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_out[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_out[3] = {0, 0, 0};

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);



  for(int32_t r = 0; r < dims[2]; ++r)
  {
    for(int32_t q = 0; q < dims[1]; ++q)
    {
      for(int32_t p = 0; p < dims[0]; ++p)
      {
        data_in[p * stride_in[0] + q * stride_in[1] + r * stride_in[2]] = distribution(generator);
      }
    }
  }

  soda::app::heat3d(
    data_in.data(),
    extent_in,
    stride_in,
    min_in,
    data_out.data(),
    extent_out,
    stride_out,
    min_out);

  int error_count = 0;



  for(int32_t r = 1; r < dims[2] - 1; ++r)
  {
    for(int32_t q = 1; q < dims[1] - 1; ++q)
    {
      for(int32_t p = 1; p < dims[0] - 1; ++p)
      {

        data_in_iter1[p*stride_in_iter1[0]+q*stride_in_iter1[1]+r*stride_in_iter1[2]] = ((.125f * data_in[(p+1)*stride_in[0] + (q+0)*stride_in[1] + (r+0)*stride_in[2]]) + (.125f * data_in[(p-1)*stride_in[0] + (q+0)*stride_in[1] + (r+0)*stride_in[2]]) + (.125f * data_in[(p+0)*stride_in[0] + (q+1)*stride_in[1] + (r+0)*stride_in[2]]) + (.125f * data_in[(p+0)*stride_in[0] + (q-1)*stride_in[1] + (r+0)*stride_in[2]]) + (.125f * data_in[(p+0)*stride_in[0] + (q+0)*stride_in[1] + (r+1)*stride_in[2]]) + (.125f * data_in[(p+0)*stride_in[0] + (q+0)*stride_in[1] + (r-1)*stride_in[2]]) + (.25f * data_in[(p+0)*stride_in[0] + (q+0)*stride_in[1] + (r+0)*stride_in[2]]));
      }
    }
  }



  for(int32_t r = 2; r < dims[2] - 2; ++r)
  {
    for(int32_t q = 2; q < dims[1] - 2; ++q)
    {
      for(int32_t p = 2; p < dims[0] - 2; ++p)
      {

        float result_out = ((.125f * data_in_iter1[(p+1)*stride_in_iter1[0] + (q+0)*stride_in_iter1[1] + (r+0)*stride_in_iter1[2]]) + (.125f * data_in_iter1[(p-1)*stride_in_iter1[0] + (q+0)*stride_in_iter1[1] + (r+0)*stride_in_iter1[2]]) + (.125f * data_in_iter1[(p+0)*stride_in_iter1[0] + (q+1)*stride_in_iter1[1] + (r+0)*stride_in_iter1[2]]) + (.125f * data_in_iter1[(p+0)*stride_in_iter1[0] + (q-1)*stride_in_iter1[1] + (r+0)*stride_in_iter1[2]]) + (.125f * data_in_iter1[(p+0)*stride_in_iter1[0] + (q+0)*stride_in_iter1[1] + (r+1)*stride_in_iter1[2]]) + (.125f * data_in_iter1[(p+0)*stride_in_iter1[0] + (q+0)*stride_in_iter1[1] + (r-1)*stride_in_iter1[2]]) + (.25f * data_in_iter1[(p+0)*stride_in_iter1[0] + (q+0)*stride_in_iter1[1] + (r+0)*stride_in_iter1[2]]));
        float val_fpga = data_out[p * stride_out[0] + q * stride_out[1] + r * stride_out[2]];
        float val_cpu = result_out;
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
}ssed/soda__heat3d/heat3d_host.cpp"
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
void heat3d_kernel(
  ap_uint<64>* bank_1_out,
  ap_uint<64>* bank_0_in,
  uint64_t coalesced_data_num);
}

namespace soda {
namespace app {

constexpr int kStencilDim0 = 5;
constexpr int kStencilDim1 = 5;
constexpr int kStencilDim2 = 5;
constexpr int kStencilDistance = 4162;
constexpr int kWidth_in = 32;
constexpr int kWidth_out = 32;


using Type_in = float;
using Type_out = float;

int heat3d(
  const Type_in* var_in_ptr,
  const int32_t var_in_extent[3],
  const int32_t var_in_stride[3],
  const int32_t var_in_min[3],
  Type_out* var_out_ptr,
  const int32_t var_out_extent[3],
  const int32_t var_out_stride[3],
  const int32_t var_out_min[3],
  // const char* bitstream,
  const int burst_width = 64,
  const int tile_size_0 = 32,
  const int tile_size_1 = 32,
  const int unroll_factor = 2)
{


  int bank_count_in = 1;
  int bank_count_out = 1;
#line 105 "soda_designs_processed/soda__heat3d/heat3d_host.cpp"
  auto round_up = [](int64_t a, int64_t b) -> int64_t { return ((a - 1) / b + 1) * b; };


  const int elem_count_per_cycle_in = burst_width / kWidth_in * bank_count_in;
  const int elem_count_per_cycle_out = burst_width / kWidth_out * bank_count_out;
  int32_t tile_count_dim_0 = (var_in_extent[0] - kStencilDim0 + 1 - 1) / (tile_size_0 - kStencilDim0 + 1) + 1;
  int32_t tile_count_dim_1 = (var_in_extent[1] - kStencilDim1 + 1 - 1) / (tile_size_1 - kStencilDim1 + 1) + 1;
  int64_t tile_count = tile_count_dim_0 * tile_count_dim_1;


  int64_t elem_count_per_tile = tile_size_0 * tile_size_1 * var_in_extent[2];
  int64_t cycle_count_per_tile = (elem_count_per_tile - 1) / elem_count_per_cycle_in + 1;
  int64_t elem_count_aligned_per_tile_i = cycle_count_per_tile * elem_count_per_cycle_in;
  int64_t elem_count_aligned_per_tile_o = cycle_count_per_tile * elem_count_per_cycle_out;


  int64_t buf_size_in = (tile_count * elem_count_aligned_per_tile_i + round_up(kStencilDistance, elem_count_per_cycle_in)) / bank_count_in * sizeof(Type_in);
  int64_t buf_size_out = (tile_count * elem_count_aligned_per_tile_o + round_up(kStencilDistance, elem_count_per_cycle_out)) / bank_count_out * sizeof(Type_out);


  vector<unique_ptr<Type_in, decltype(&free)>> buf_in;
  buf_in.reserve(bank_count_in);
  for (int bank = 0; bank < bank_count_in; ++bank) {
    buf_in.emplace_back(static_cast<Type_in*>(aligned_alloc(4096, round_up(buf_size_in, 4096))), &free);
  }
  vector<unique_ptr<Type_out, decltype(&free)>> buf_out;
  buf_out.reserve(bank_count_out);
  for (int bank = 0; bank < bank_count_out; ++bank) {
    buf_out.emplace_back(static_cast<Type_out*>(aligned_alloc(4096, round_up(buf_size_out, 4096))), &free);
  }


  for(int32_t tile_index_dim_1 = 0; tile_index_dim_1 < tile_count_dim_1; ++tile_index_dim_1)
  {
    int32_t actual_tile_size_dim_1 = (tile_index_dim_1==tile_count_dim_1-1) ? var_in_extent[1] - (tile_size_1 - kStencilDim1 + 1) * tile_index_dim_1 : tile_size_1;
    for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
    {
      int32_t actual_tile_size_dim_0 = (tile_index_dim_0==tile_count_dim_0-1) ? var_in_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

      for(int32_t k = 0; k < var_in_extent[2]; ++k)
      {
        for(int32_t j = 0; j < actual_tile_size_dim_1; ++j)
        {
          for(int32_t i = 0; i < actual_tile_size_dim_0; ++i)
          {



            int32_t burst_index = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1) / elem_count_per_cycle_in;
            int32_t burst_residue = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1) % elem_count_per_cycle_in;
            int32_t p = tile_index_dim_0 * (tile_size_0 - kStencilDim0) + i;
            int32_t q = tile_index_dim_1 * (tile_size_1 - kStencilDim1) + j;
            int32_t r = k;
            int64_t tiled_offset = (tile_index_dim_0 + tile_count_dim_0 * tile_index_dim_1) * elem_count_aligned_per_tile_i + burst_index * elem_count_per_cycle_in + burst_residue;
            int64_t original_offset = p * var_in_stride[0] + q * var_in_stride[1] + r * var_in_stride[2];
            buf_in[tiled_offset % bank_count_in].get()[tiled_offset / bank_count_in] = var_in_ptr[std::max(int64_t(0), original_offset - 0)];
          }
        }
      }
    }
  }

  clog << "INFO: tile_count[0] = " << tile_count_dim_0 << ", tile_size[0] = " << tile_size_0 << endl;
  clog << "INFO: tile_count[1] = " << tile_count_dim_1 << ", tile_size[1] = " << tile_size_1 << endl;
  clog << "INFO: in.extent[0] = " << var_in_extent[0] << ", in.extent[1] = " << var_in_extent[1] << ", in.extent[2] = " << var_in_extent[2] << "" << endl;
  clog << "INFO: in.stride[0] = " << var_in_stride[0] << ", in.stride[1] = " << var_in_stride[1] << ", in.stride[2] = " << var_in_stride[2] << "" << endl;
  clog << "INFO: in.min[0] = " << var_in_min[0] << ", in.min[1] = " << var_in_min[1] << ", in.min[2] = " << var_in_min[2] << "" << endl;
  clog << "INFO: out.extent[0] = " << var_out_extent[0] << ", out.extent[1] = " << var_out_extent[1] << ", out.extent[2] = " << var_out_extent[2] << "" << endl;
  clog << "INFO: out.stride[0] = " << var_out_stride[0] << ", out.stride[1] = " << var_out_stride[1] << ", out.stride[2] = " << var_out_stride[2] << "" << endl;
  clog << "INFO: out.min[0] = " << var_out_min[0] << ", out.min[1] = " << var_out_min[1] << ", out.min[2] = " << var_out_min[2] << "" << endl;

  int64_t tile_data_count = ((int64_t(var_in_extent[2]) * tile_size_0 * tile_size_1 - 1) / elem_count_per_cycle_in + 1) * elem_count_per_cycle_in / unroll_factor;
  int64_t cycle_count = ((int64_t(var_in_extent[2]) * tile_size_0 * tile_size_1 * tile_count_dim_0 * tile_count_dim_1 + kStencilDistance - 1) / elem_count_per_cycle_in + 1);
  clog << "INFO: tile_data_count = " << tile_data_count << ", cycle_count = " << cycle_count << endl;


  ::heat3d_kernel(
    (ap_uint<64>*)buf_out[0].get(),
    (ap_uint<64>*)buf_in[0].get(),
    cycle_count);
#line 221 "soda_designs_processed/soda__heat3d/heat3d_host.cpp"
  for(int32_t tile_index_dim_1 = 0; tile_index_dim_1 < tile_count_dim_1; ++tile_index_dim_1)
  {
    int32_t actual_tile_size_dim_1 = (tile_index_dim_1 == tile_count_dim_1-1) ? var_in_extent[1] - (tile_size_1 - kStencilDim1 + 1) * tile_index_dim_1 : tile_size_1;
    for(int32_t tile_index_dim_0 = 0; tile_index_dim_0 < tile_count_dim_0; ++tile_index_dim_0)
    {
      int32_t actual_tile_size_dim_0 = (tile_index_dim_0 == tile_count_dim_0-1) ? var_in_extent[0] - (tile_size_0 - kStencilDim0 + 1) * tile_index_dim_0 : tile_size_0;

      for(int32_t k = 2; k < var_out_extent[2] - 2; ++k)
      {
        for(int32_t j = 2; j < actual_tile_size_dim_1 - 2; ++j)
        {
          for(int32_t i = 2; i < actual_tile_size_dim_0 - 2; ++i)
          {



            int32_t p = tile_index_dim_0 * (tile_size_0-kStencilDim0 + 1) + i;
            int32_t q = tile_index_dim_1 * (tile_size_1-kStencilDim1 + 1) + j;
            int32_t r = k;
            int64_t original_offset = p * var_out_stride[0] + q * var_out_stride[1] + r * var_out_stride[2];
            int32_t burst_index_out = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1 + 2048) / elem_count_per_cycle_out;
            int32_t burst_residue_out = (i + j * tile_size_0 + k * tile_size_0 * tile_size_1 + 2048) % elem_count_per_cycle_out;
            int64_t tiled_offset_out = (tile_index_dim_0+tile_count_dim_0 * tile_index_dim_1) * elem_count_aligned_per_tile_o + burst_index_out * elem_count_per_cycle_out + burst_residue_out;
            var_out_ptr[original_offset] = buf_out[tiled_offset_out % bank_count_out].get()[tiled_offset_out / bank_count_out];
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

  vector<float> data_in(dims[0] * dims[1] * dims[2]);
  int32_t extent_in[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_in[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_in[3] = {0, 0, 0};
  vector<float> data_in_iter1(dims[0] * dims[1] * dims[2]);
  int32_t extent_in_iter1[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_in_iter1[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_in_iter1[3] = {0, 0, 0};
  vector<float> data_out(dims[0] * dims[1] * dims[2]);
  int32_t extent_out[3] = {dims[0], dims[1], dims[2]};
  int32_t stride_out[3] = {1, dims[0], dims[0] * dims[1]};
  int32_t min_out[3] = {0, 0, 0};

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0, 1.0);



  for(int32_t r = 0; r < dims[2]; ++r)
  {
    for(int32_t q = 0; q < dims[1]; ++q)
    {
      for(int32_t p = 0; p < dims[0]; ++p)
      {
        data_in[p * stride_in[0] + q * stride_in[1] + r * stride_in[2]] = distribution(generator);
      }
    }
  }

  soda::app::heat3d(
    data_in.data(),
    extent_in,
    stride_in,
    min_in,
    data_out.data(),
    extent_out,
    stride_out,
    min_out);

  int error_count = 0;



  for(int32_t r = 1; r < dims[2] - 1; ++r)
  {
    for(int32_t q = 1; q < dims[1] - 1; ++q)
    {
      for(int32_t p = 1; p < dims[0] - 1; ++p)
      {

        data_in_iter1[p*stride_in_iter1[0]+q*stride_in_iter1[1]+r*stride_in_iter1[2]] = ((.125f * data_in[(p+1)*stride_in[0] + (q+0)*stride_in[1] + (r+0)*stride_in[2]]) + (.125f * data_in[(p-1)*stride_in[0] + (q+0)*stride_in[1] + (r+0)*stride_in[2]]) + (.125f * data_in[(p+0)*stride_in[0] + (q+1)*stride_in[1] + (r+0)*stride_in[2]]) + (.125f * data_in[(p+0)*stride_in[0] + (q-1)*stride_in[1] + (r+0)*stride_in[2]]) + (.125f * data_in[(p+0)*stride_in[0] + (q+0)*stride_in[1] + (r+1)*stride_in[2]]) + (.125f * data_in[(p+0)*stride_in[0] + (q+0)*stride_in[1] + (r-1)*stride_in[2]]) + (.25f * data_in[(p+0)*stride_in[0] + (q+0)*stride_in[1] + (r+0)*stride_in[2]]));
      }
    }
  }



  for(int32_t r = 2; r < dims[2] - 2; ++r)
  {
    for(int32_t q = 2; q < dims[1] - 2; ++q)
    {
      for(int32_t p = 2; p < dims[0] - 2; ++p)
      {

        float result_out = ((.125f * data_in_iter1[(p+1)*stride_in_iter1[0] + (q+0)*stride_in_iter1[1] + (r+0)*stride_in_iter1[2]]) + (.125f * data_in_iter1[(p-1)*stride_in_iter1[0] + (q+0)*stride_in_iter1[1] + (r+0)*stride_in_iter1[2]]) + (.125f * data_in_iter1[(p+0)*stride_in_iter1[0] + (q+1)*stride_in_iter1[1] + (r+0)*stride_in_iter1[2]]) + (.125f * data_in_iter1[(p+0)*stride_in_iter1[0] + (q-1)*stride_in_iter1[1] + (r+0)*stride_in_iter1[2]]) + (.125f * data_in_iter1[(p+0)*stride_in_iter1[0] + (q+0)*stride_in_iter1[1] + (r+1)*stride_in_iter1[2]]) + (.125f * data_in_iter1[(p+0)*stride_in_iter1[0] + (q+0)*stride_in_iter1[1] + (r-1)*stride_in_iter1[2]]) + (.25f * data_in_iter1[(p+0)*stride_in_iter1[0] + (q+0)*stride_in_iter1[1] + (r+0)*stride_in_iter1[2]]));
        float val_fpga = data_out[p * stride_out[0] + q * stride_out[1] + r * stride_out[2]];
        float val_cpu = result_out;
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
