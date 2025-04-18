///////////////////////////////////////////////////////////////////////////////
// Course:      ECE8893 - Parallel Programming for FPGAs
// Filename:    sim.cpp
// Description: Test bench for tiling-based convolution
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
float conv_layer_input_feature_map[IN_FM_DEPTH][IN_FM_HEIGHT][IN_FM_WIDTH];
float conv_layer_weights[OUT_FM_DEPTH][IN_FM_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH];
float conv_layer_bias[OUT_FM_DEPTH];
float conv_layer_golden_output_feature_map[OUT_FM_DEPTH][OUT_FM_HEIGHT][OUT_FM_WIDTH];

fm_t fixp_conv_layer_input_feature_map[IN_FM_DEPTH][IN_FM_HEIGHT][IN_FM_WIDTH];
wt_t fixp_conv_layer_weights[OUT_FM_DEPTH][IN_FM_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH];
wt_t fixp_conv_layer_bias[OUT_FM_DEPTH];
fm_t fixp_conv_layer_output_feature_map[OUT_FM_DEPTH][OUT_FM_HEIGHT][OUT_FM_WIDTH] = {0};

fm_t fixp_conv_layer_input_feature_map_outL4[OUT_FM_DEPTH][OUT_FM_HEIGHT][OUT_FM_WIDTH] = {0};
fm_t fixp_conv_layer_output_L6_concat[2 * OUT_FM_DEPTH][OUT_FM_HEIGHT][OUT_FM_WIDTH] = {0};
fm_t fixp_conv_layer3_output[OUT_FM_DEPTH][OUT_FM_HEIGHT][OUT_FM_WIDTH] = {0};
fm_t maxpool_output_layer[2 * OUT_FM_DEPTH][OUT_FM_HEIGHT / 2][OUT_FM_WIDTH / 2] = {0};

fm_t maxpool_output_layer_golden[2 * OUT_FM_DEPTH][OUT_FM_HEIGHT / 2][OUT_FM_WIDTH / 2] = {0};
//--------------------------------------------------------------------------
// Read the reference files into test bench arrays
//--------------------------------------------------------------------------
void read_bin_files()
{
    // Input Feature Map
    ifstream ifs_conv_input("./bin/layer5_output_concat.bin", ios::in | ios::binary);
    // ifstream ifs_conv_input("./bin/conv_layer6_input2.bin", ios::in | ios::binary);
    ifs_conv_input.read((char *)(**conv_layer_input_feature_map), IN_FM_DEPTH * IN_FM_HEIGHT * IN_FM_WIDTH * sizeof(float));
    ifs_conv_input.close();

    // Typecast to fixed-point
    for (int c = 0; c < IN_FM_DEPTH; c++)
        for (int i = 0; i < IN_FM_HEIGHT; i++)
            for (int j = 0; j < IN_FM_WIDTH; j++)
                fixp_conv_layer_input_feature_map[c][i][j] = (fm_t)conv_layer_input_feature_map[c][i][j];

    // Weights
    ifstream ifs_conv_weights("./bin/fused_conv6_bn6_weights.bin", ios::in | ios::binary);
    ifs_conv_weights.read((char *)(***conv_layer_weights), OUT_FM_DEPTH * IN_FM_DEPTH * KERNEL_HEIGHT * KERNEL_WIDTH * sizeof(float));
    ifs_conv_weights.close();

    // Typecast to fixed-point
    for (int f = 0; f < OUT_FM_DEPTH; f++)
        for (int c = 0; c < IN_FM_DEPTH; c++)
            for (int m = 0; m < KERNEL_HEIGHT; m++)
                for (int n = 0; n < KERNEL_WIDTH; n++)
                    fixp_conv_layer_weights[f][c][m][n] = (wt_t)conv_layer_weights[f][c][m][n];

    // Bias
    ifstream ifs_conv_bias("./bin/fused_conv6_bn6_bias.bin", ios::in | ios::binary);
    ifs_conv_bias.read((char *)(conv_layer_bias), OUT_FM_DEPTH * sizeof(float));
    ifs_conv_bias.close();

    // Typecast to fixed-point
    for (int f = 0; f < OUT_FM_DEPTH; f++)
        fixp_conv_layer_bias[f] = (wt_t)conv_layer_bias[f];

    // Golden Output
    ifstream ifs_golden_output("./bin/conv_layer6_output.bin", ios::in | ios::binary);
    ifs_golden_output.read((char *)(**conv_layer_golden_output_feature_map), OUT_FM_DEPTH * OUT_FM_HEIGHT * OUT_FM_WIDTH * sizeof(float));
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

    // Compute Mean-Squared-Error
    for (int f = 0; f < OUT_FM_DEPTH; f++)
    {
        for (int i = 0; i < OUT_FM_HEIGHT; i++)
        {
            for (int j = 0; j < OUT_FM_WIDTH; j++)
            {
                mse += std::pow((conv_layer_golden_output_feature_map[f][i][j] - (float)fixp_conv_layer_output_feature_map[f][i][j]), 2);
            }
        }
#ifdef PRINT_DEBUG
        // Prints sample output values (first feature of each channel) for comparison
        // Modify as required for debugging
        int row = 103;
        int col = 0;

        cout << "Output feature[" << f << "][" << row << "][" << col << "]: ";
        cout << "Expected: " << conv_layer_golden_output_feature_map[f][row][col] << "\t";
        cout << "Actual: " << fixp_conv_layer_output_feature_map[f][row][col];
        cout << endl;
#endif
    }

    mse = mse / (OUT_FM_DEPTH * OUT_FM_HEIGHT * OUT_FM_WIDTH);

    std::cout << "\nOutput MSE:  " << mse << std::endl;

    std::cout << "----------------------------------------" << std::endl;
#ifdef CSIM_DEBUG
    if (mse > 0 && mse < std::pow(10, -11))
    {
        std::cout << "Floating-point Simulation Layer6 SUCCESSFUL!!!" << std::endl;
    }
    else
    {
        std::cout << "Floating-point Simulation Layer6 FAILED :(" << std::endl;
    }
#else
    if (mse > 0 && mse < std::pow(10, -3))
    {
        std::cout << "Fixed-point Simulation Layer6 SUCCESSFUL!!!" << std::endl;
    }
    else
    {
        std::cout << "Fixed-point Simulation Layer6 FAILED :(" << std::endl;
    }
#endif
    std::cout << "----------------------------------------" << std::endl;
    /*
        for (int f = 0; f < OUT_FM_DEPTH; f++)
        {
            for (int i = 0; i < OUT_FM_HEIGHT; i++)
            {
                for (int j = 0; j < OUT_FM_WIDTH; j++)
                {
                    if (fixp_conv_layer_input_feature_map_outL4[f][i][j] != conv_layer_input_feature_map[f][i][j])
                    {
                        cout << "Output feature[" << f << "][" << i << "][" << j << "]: ";
                        cout << "Expected: " << conv_layer_input_feature_map[f][i][j] << "\t";
                        cout << "Actual: " << fixp_conv_layer_input_feature_map_outL4[f][i][j];
                        cout << endl;
                    }
                }
            }
        }

            // Split output into 2
            for (int f = 0; f < OUT_FM_DEPTH / 2; f++)
            {
                for (int i = 0; i < OUT_FM_HEIGHT; i++)
                {
                    for (int j = 0; j < OUT_FM_WIDTH; j++)
                    {
                        fixp_conv_layer_output_feature_map_split[f][i][j] = fixp_conv_layer_output_feature_map[f + 32][i][j];
                    }
                }
            }
            // Convert to bin file
            // open the binary file for writing
            ofstream outfile1("./bin/layer3_output.bin", ios::binary);
            outfile1.write((char *)(**fixp_conv_layer_output_feature_map), OUT_FM_DEPTH * OUT_FM_HEIGHT * OUT_FM_WIDTH * sizeof(float));
            outfile1.close();
            ofstream outfile2("./bin/layer3_output_split.bin", ios::binary);
            outfile2.write((char *)(**fixp_conv_layer_output_feature_map_split), OUT_FM_DEPTH / 2 * OUT_FM_HEIGHT * OUT_FM_WIDTH * sizeof(float));
            outfile2.close();

*/
    // CONCAT LAYER3 OUTPUT with LAYER6 OUTPUT

    ifstream ifs_conv_input_4("./bin/layer3_output.bin", ios::in | ios::binary);
    ifs_conv_input_4.read((char *)(**fixp_conv_layer3_output), IN_FM_DEPTH * IN_FM_HEIGHT * IN_FM_WIDTH * sizeof(float));
    ifs_conv_input_4.close();

    for (int f = 0; f < 2 * OUT_FM_DEPTH; f++)

        for (int i = 0; i < OUT_FM_HEIGHT; i++)

            for (int j = 0; j < OUT_FM_WIDTH; j++)
            {
                if (f < 64)
                    fixp_conv_layer_output_L6_concat[f][i][j] = fixp_conv_layer_output_feature_map[f][i][j];
                else
                    fixp_conv_layer_output_L6_concat[f][i][j] = fixp_conv_layer3_output[f - 64][i][j];
            }

    // Convert to bin file
    // open the binary file for writing
    ofstream outfile1("./bin/layer6_output_concat.bin", ios::binary);
    outfile1.write((char *)(**fixp_conv_layer_output_L6_concat), (2 * OUT_FM_DEPTH) * OUT_FM_HEIGHT * OUT_FM_WIDTH * sizeof(float));
    outfile1.close();

    // MAXPOOL
    for (int i = 0; i < 2 * OUT_FM_DEPTH; i++)
        for (int j = 0; j < OUT_FM_HEIGHT / 2; j++)
            for (int k = 0; k < OUT_FM_WIDTH / 2; k++)
            {
                fm_t max1 = fixp_conv_layer_output_L6_concat[i][j * 2][k * 2];
                for (int h = 0; h < 2; h++)
                {
                    for (int w = 0; w < 2; w++)
                    {

                        if (max1 < fixp_conv_layer_output_L6_concat[i][j * 2 + h][k * 2 + w])
                            max1 = fixp_conv_layer_output_L6_concat[i][j * 2 + h][k * 2 + w];
                    }
                }
                maxpool_output_layer[i][j][k] = max1;
            }

    ifstream ifs_maxpool_out("./bin/maxpool_output.bin", ios::in | ios::binary);
    ifs_maxpool_out.read((char *)(**maxpool_output_layer_golden), 2 * IN_FM_DEPTH * IN_FM_HEIGHT / 2 * IN_FM_WIDTH / 2 * sizeof(float));
    ifs_maxpool_out.close();
    /*
        float mse2 = 0.0;
        // Compute Mean-Squared-Error
        for (int f = 0; f < 128; f++)
        {
            for (int i = 0; i < 52; i++)
            {
                for (int j = 0; j < 52; j++)
                {
                    mse2 += std::pow((maxpool_output_layer[f][i][j] - maxpool_output_layer_golden[f][i][j]), 2);
                }
            }
        }
        mse2 = mse2 / (128 * 52 * 52);

        std::cout << "\nMAxpool MSE:  " << mse2 << std::endl;
    */
    std::cout << "----------------------------------------" << std::endl;
    return 0;
}
