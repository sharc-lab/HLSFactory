///////////////////////////////////////////////////////////////////////////////
// Course:      ECE8893 - Parallel Programming for FPGAs
// Filename:    sim.cpp
// Description: Test bench for tiling-based convolution
//
// Note:        You are not required to turn in this code.
//              In case you make any modifications (for debugging, etc.),
//              do ensure your design works with the original test bench.
//
//              For debugging, you may use the PRINT_DEBUG construct.
//              You can put your print statements inside this ifdef block and
//              use "make debug".
///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <cmath>

#include "conv.h"

using namespace std;

//--------------------------------------------------------------------------
// Convolution layer inputs, parameters, and reference output
//--------------------------------------------------------------------------
float conv_layer_input_feature_map[3][416][416];
float conv_layer_weights[32][3][3][3];
float conv_layer_bias[32];
float conv_layer_golden_output_feature_map[32][208][208];

fm_t fixp_conv_layer_input_feature_map[3][416][416];
wt_t fixp_conv_layer_weights[32][3][3][3];
wt_t fixp_conv_layer_bias[32];
fm_t fixp_conv_layer_output_feature_map[32][208][208] = {0};

//--------------------------------------------------------------------------
// Read the reference files into test bench arrays
//--------------------------------------------------------------------------
void read_bin_files()
{
    // Input Feature Map
    ifstream ifs_conv_input("./bin/conv_input_layer1.bin", ios::in | ios::binary);
    ifs_conv_input.read((char *)(**conv_layer_input_feature_map), 3 * 416 * 416 * sizeof(float));
    ifs_conv_input.close();

    // Typecast to fixed-point
    for (int c = 0; c < 3; c++)
        for (int i = 0; i < 416; i++)
            for (int j = 0; j < 416; j++)
            {
                fixp_conv_layer_input_feature_map[c][i][j] = (fm_t)conv_layer_input_feature_map[c][i][j];
                // std::cout << "Debug input values " << fixp_conv_layer_input_feature_map[c][i][j] << std::endl;
            }

    // Weights
    ifstream ifs_conv_weights("./bin/fused_weights_layer1.bin", ios::in | ios::binary);
    ifs_conv_weights.read((char *)(**conv_layer_weights), 32 * 3 * 3 * 3 * sizeof(float));
    ifs_conv_weights.close();

    // Typecast to fixed-point
    for (int f = 0; f < 32; f++)
        for (int c = 0; c < 3; c++)
            for (int m = 0; m < 3; m++)
                for (int n = 0; n < 3; n++)
                    fixp_conv_layer_weights[f][c][m][n] = (wt_t)conv_layer_weights[f][c][m][n];

    // Bias
    // This is done so that the bias values are all 0
    ifstream ifs_conv_bias("./bin/fused_bias_layer1.bin", ios::in | ios::binary);
    ifs_conv_bias.read((char *)(conv_layer_bias), 32 * sizeof(float));
    ifs_conv_bias.close();

    // Typecast to fixed-point
    for (int f = 0; f < 32; f++)
        fixp_conv_layer_bias[f] = (wt_t)conv_layer_bias[f];

    // Golden Output Testing with batch normalization fused
    ifstream ifs_golden_output("./bin/conv_output_layer1.bin", ios::in | ios::binary);
    ifs_golden_output.read((char *)(**conv_layer_golden_output_feature_map), 32 * 208 * 208 * sizeof(float));
    ifs_golden_output.close();
}

//--------------------------------------------------------------------------
// This is where the real fun begins.
//--------------------------------------------------------------------------
int main()
{
    long double mse = 0.0;

    // Read reference inputs, parameters, and output
    read_bin_files();

    std::cout << "Beginning HLS tiled-convolution simulation..." << std::endl;

    tiled_conv(fixp_conv_layer_input_feature_map,
               fixp_conv_layer_weights,
               fixp_conv_layer_bias,
               fixp_conv_layer_output_feature_map);

    std::cout << "Tiled-convolution simulation complete!\n"
              << std::endl;

    // Compute Mean-Squared-Error of output
    for (int f = 0; f < 32; f++)
    {
        for (int i = 0; i < 208; i++)
        {
            for (int j = 0; j < 208; j++)
            {
                mse += std::pow((conv_layer_golden_output_feature_map[f][i][j] - (float)fixp_conv_layer_output_feature_map[f][i][j]), 2);
            }
        }
#ifdef PRINT_DEBUG
        // Prints sample output values (first feature of each channel) for comparison
        // Modify as required for debugging
        int row = 207;
        int col = 0;

        cout << "Output feature[" << f << "][" << row << "][" << col << "]: ";
        cout << "Expected: " << conv_layer_golden_output_feature_map[f][row][col] << "\t";
        cout << "Actual: " << fixp_conv_layer_output_feature_map[f][row][col];
        cout << endl;
#endif
    }

    mse = mse / (32 * 208 * 208);

    std::cout << "\nOutput MSE:  " << mse << std::endl;

    std::cout << "----------------------------------------" << std::endl;
#ifdef CSIM_DEBUG
    if (mse > 0 && mse < std::pow(10, -12))
    {
        std::cout << "Floating-point Simulation Layer1 SUCCESSFUL!!!" << std::endl;
    }
    else
    {
        std::cout << "Floating-point Simulation Layer1 FAILED :(" << std::endl;
    }
#else
    if (mse > 0 && mse < std::pow(10, -3))
    {
        std::cout << "Fixed-point Simulation Layer1 SUCCESSFUL!!!" << std::endl;
    }
    else
    {
        std::cout << "Fixed-point Simulation Layer1 FAILED :(" << std::endl;
    }
#endif
    std::cout << "----------------------------------------" << std::endl;

    // Extracting output layer
    ofstream outfile1("./bin/layer1_output.bin", ios::binary);
    outfile1.write((char *)(**fixp_conv_layer_output_feature_map), OUT_FM_DEPTH * OUT_FM_HEIGHT * OUT_FM_WIDTH * sizeof(fm_t));
    outfile1.close();

    return 0;
}
