#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cmath>

#include <torch/torch.h>

#include "cim_args.h"
#include "conv.h"

using namespace std;

int input2d[IN_ROWS][IN_COLS];
int weight2d[WT_ROWS][WT_COLS];
float weight2d_cond[WT_ROWS][WT_BIN_COLS];
float correct_output[IN_ROWS][WT_COLS];

string path_to_repo = "/usr/scratch/vaidehi/fpga-project-spring2023";

// imports inputs and weights from binary files
void read_bin_files()
{
  ifstream input2d_file;
  input2d_file.open(path_to_repo + "/bin/quant_input_c.bin", ios::in | ios::binary);
  input2d_file.read(reinterpret_cast<char*>(&input2d), IN_ROWS*IN_COLS*sizeof(int));
  input2d_file.close();

  ifstream weight2d_file;
  weight2d_file.open(path_to_repo + "/bin/quant_weight_c.bin", ios::in | ios::binary);
  weight2d_file.read(reinterpret_cast<char*>(&weight2d), WT_ROWS*WT_COLS*sizeof(int));
  weight2d_file.close();

  ifstream weight2d_cond_file;
  weight2d_cond_file.open(path_to_repo + "/bin/weights_as_conductances_c.bin", ios::in | ios::binary);
  weight2d_cond_file.read(reinterpret_cast<char*>(&weight2d_cond), WT_ROWS*WT_BIN_COLS*sizeof(float));
  weight2d_cond_file.close();

  ifstream correct_output_file;
  correct_output_file.open(path_to_repo + "/bin/correct_output_c.bin", ios::in | ios::binary);
  correct_output_file.read(reinterpret_cast<char*>(&correct_output), IN_ROWS*WT_COLS*sizeof(float));
  correct_output_file.close();
}

// calculates optimal value of resistive divider
torch::Tensor r_opt(torch::Tensor r1, torch::Tensor r2)
{
  torch::Tensor result = (r1-r2*sqrt(r1/r2))/(sqrt(r1/r2)-1);
  return result;
}

// compute adc output
torch::Tensor adc_output(CIM_Args *args, torch::Tensor inputs, torch::Tensor weights)
{
  torch::Tensor equiv_cond = torch::matmul(inputs, weights);
  torch::Tensor bl_voltages = torch::div(torch::tensor(args->m_vdd), 1 + torch::mul(equiv_cond, args->m_res_divider));
  torch::Tensor adc_out = args->m_num_refs - torch::bucketize(bl_voltages, args->m_vref.flip(0), true);

  return adc_out;
}

// main loop
int main()
{
  read_bin_files();

  CIM_Args *args = new CIM_Args();

  // test binary file readins
  //for (int i=0; i<3; i++) {
  //  for (int j=0; j<3; j++) {
  //    cout << weight2d_cond[i][j] << " ";
  //  }
  //  cout << endl;
  //}

  float num_refs = pow(2, args->m_adc_precision);
  args->setNumRefs(num_refs);

  torch::Tensor x = torch::arange(num_refs) + 1;

  torch::Tensor LRS = args->m_mem_values[1];
  torch::Tensor HRS = args->m_mem_values[0];

  torch::Tensor r_max = 1/(x/LRS);

  torch::Tensor r_min = 1/((args->m_open_rows-(x-1))/HRS + (x-1)/LRS);

  torch::Tensor res_divider = (r_opt(r_min[0], r_max[0]) + r_opt(r_min[args->m_open_rows-1], r_max[args->m_open_rows]))/2;

  torch::Tensor v_max = (args->m_vdd)*(r_max/(res_divider + r_max));
  torch::Tensor v_min = (args->m_vdd)*(r_min/(res_divider + r_min));

  torch::Tensor v_ref = (v_min + v_max)/2;

  // update args
  args->setResDivider(res_divider);
  args->setVRef(v_ref);

  torch::Tensor inputs = torch::from_blob(input2d, {IN_ROWS, IN_COLS}, torch::kInt);
  torch::Tensor inputs_f = inputs.to(torch::kFloat);
  torch::Tensor weights = torch::from_blob(weight2d_cond, {WT_ROWS, WT_BIN_COLS});

  torch::Tensor adc_out = torch::zeros({IN_ROWS, WT_BIN_COLS});
  torch::Tensor psum = torch::zeros_like(adc_out);

  // divide the weight matrix into partitions
  int num_partitions = ceil(WT_ROWS/args->m_open_rows);

  for (int i = 0; i < args->m_input_precision; i++) {
    int mask = pow(2, i);

    torch::Tensor input = torch::bitwise_right_shift(torch::bitwise_and(inputs, mask), i).to(torch::kFloat);

    psum = torch::zeros_like(adc_out);
    for (int j = 0; j < num_partitions; j++) {
      torch::Tensor out = adc_output(args, input, weights);
      psum += out;
    }

    psum *= mask;

    adc_out += psum;
  }

  int max_val = pow(2, args->m_weight_precision);
  int base = args->m_mem_values.size(0);
  int cols_per_weight = ceil(log(max_val)/log(base));
  torch::Tensor weights_mask = pow(base, torch::arange(cols_per_weight).flip(0));

  torch::Tensor adc_out_reshaped = adc_out.view({adc_out.size(0), adc_out.size(1)/cols_per_weight, cols_per_weight});

  adc_out_reshaped *= weights_mask;

  torch::Tensor output = adc_out_reshaped.sum(/*dim*/-1);

  torch::Tensor expected_outputs = torch::from_blob(correct_output, {IN_ROWS, WT_COLS});
  
  if (torch::allclose(output, expected_outputs)) {
    cout << "MATCH! Outputs are correct" << endl;
  }
  else {
    cout << "MISMATCH! Outputs are incorrect" << endl;
  }

  return 0;
}
