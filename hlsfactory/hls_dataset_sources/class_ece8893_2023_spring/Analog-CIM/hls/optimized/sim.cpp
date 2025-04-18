// #include "cim_args.h"
#include "conv.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cmath>

using namespace std;

int input2d[IN_ROWS][IN_COLS];
float weight2d_cond[WT_ROWS][WT_BIN_COLS];
float v_ref[ADC_LEVELS];
int output[IN_ROWS][WT_COLS] = {0}; // initialize output to 0

int m_input2d[IN_ROWS][IN_COLS];
f_t m_weight2d_cond[WT_ROWS][WT_BIN_COLS];
f_t m_v_ref[ADC_LEVELS];
int m_output[IN_ROWS][WT_COLS];

int correct_output[IN_ROWS][WT_BIN_COLS];

string path_to_repo = "/usr/scratch/vaidehi1/fpga-project-spring2023/";

void read_csv_files() {
  ifstream input_file(path_to_repo + "csv/input2d.csv");
  string line;
  int row = 0;
  while (getline(input_file, line)) {
      stringstream ss(line);
      string cell;
      int col = 0;
      while (getline(ss, cell, ',')) {
          input2d[row][col] = stoi(cell);
          ++col;
      }
      ++row;
  }

  // typecast to fixed-point
  for (int i = 0; i < IN_ROWS; i++) {
    for (int j = 0; j < IN_COLS; j++) {
      m_input2d[i][j] = (int) input2d[i][j];
    }
  }

  // print the first 100 inputs
  //for (int i = 0; i < 100; i++) {
  //  cout << m_input2d[0][i] << " ";
  //}
  //cout << endl;

  ifstream weight_file(path_to_repo + "csv/weight2d_cond.csv");
  line = "";
  row = 0;
  while (getline(weight_file, line)) {
      stringstream ss(line);
      string cell;
      int col = 0;
      while (getline(ss, cell, ',')) {
          weight2d_cond[row][col] = stof(cell);
          ++col;
      }
      ++row;
  }

  // typecast to fixed-point
  for (int i = 0; i < WT_ROWS; i++) {
    for (int j = 0; j < WT_BIN_COLS; j++) {
      m_weight2d_cond[i][j] = (f_t) weight2d_cond[i][j];
    }
  }

  ifstream vref_file(path_to_repo + "csv/v_ref.csv");
  line = "";
  row = 0;
  while (getline(vref_file, line)) {
      stringstream ss(line);
      string cell;
      int col = 0;
      while (getline(ss, cell, ',')) {
          v_ref[row] = stof(cell);
          ++col;
      }
      ++row;
  }

  // typecast to fixed-point
  for (int i = 0; i < ADC_LEVELS; i++) {
    m_v_ref[i] = (f_t) v_ref[i];
  }

  // print v_ref
  //for (int i = 0; i < ADC_LEVELS; i++) {
  //  cout << m_v_ref[i] << endl;
  //}

  ifstream output_file(path_to_repo + "csv/correct_output.csv");
  line = "";
  row = 0;
  while (getline(output_file, line)) {
      stringstream ss(line);
      string cell;
      int col = 0;
      while (getline(ss, cell, ',')) {
          correct_output[row][col] = stoi(cell);
          ++col;
      }
      ++row;
  }
}

// main loop
int main()
{
  read_csv_files();

  // run the simulation
  int cim_args[] = {VDD, RES_DIVIDER};
  tiled_cim_conv(m_input2d, m_weight2d_cond, m_v_ref, m_output, cim_args);

  // compare to correct output
  int num_errors = 0;
  int num_correct = 0;
  for (int i = 0; i < IN_ROWS; i++) {
    for (int j = 0; j < WT_COLS; j++) {
      if (m_output[i][j] != correct_output[i][j]) {
        if (num_errors < 25) cout << "Error at (" << i << ", " << j << "): " << m_output[i][j] << " != " << correct_output[i][j] << endl;
        num_errors++;
      }
      else if (m_output[i][j] == correct_output[i][j]) {
        if (num_correct < 25) cout << "Correct at (" << i << ", " << j << "): " << m_output[i][j] << " == " << correct_output[i][j] << endl;
        num_correct++;
      }
    }
  }

  cout << "Simulation finished! Total number of errors: " << num_errors << endl;
}
