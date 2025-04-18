
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include <bitset>
#include "conv.h"

// #define PRINT_DEBUG
using namespace std;

//--------------------------------------------------------------------------
// Convolution layer inputs, parameters, and reference output
//--------------------------------------------------------------------------
float conv_layer_input_feature_map[3][736][1280];
float conv_layer_weights[64][3][7][7];
float conv_layer_bias[64];
float conv_layer_golden_output_feature_map[64][368][640];
ap_fixed<16, 3> data[64 * 368 * 640];
float golden_data[64 * 368 * 640];

fm_t fixp_conv_layer_input_feature_map[3][736][1280];
wt_t fixp_conv_layer_weights[64][3][7][7];
wt_t fixp_conv_layer_bias[64];

fm_t fixp_conv_layer_output_feature_map[64][368][640] = {0};

//--------------------------------------------------------------------------
// Read the reference files into test bench arrays
//--------------------------------------------------------------------------
void read_bin_files()
{
    // Input Feature Map
    ifstream ifs_conv_input("../bin/conv_input.bin", ios::in | ios::binary);
    ifs_conv_input.read((char *)(**conv_layer_input_feature_map), 3 * 736 * 1280 * sizeof(float));
    ifs_conv_input.close();

    // Typecast to fixed-point
    for (int c = 0; c < 3; c++)
        for (int i = 0; i < 736; i++)
            for (int j = 0; j < 1280; j++)
                fixp_conv_layer_input_feature_map[c][i][j] = (fm_t)conv_layer_input_feature_map[c][i][j];

    // Weights
    ifstream ifs_conv_weights("../bin/conv_weights.bin", ios::in | ios::binary);
    ifs_conv_weights.read((char *)(***conv_layer_weights), 64 * 3 * 7 * 7 * sizeof(float));
    ifs_conv_weights.close();

    // Typecast to fixed-point
    for (int f = 0; f < 64; f++)
        for (int c = 0; c < 3; c++)
            for (int m = 0; m < 7; m++)
                for (int n = 0; n < 7; n++)
                    fixp_conv_layer_weights[f][c][m][n] = (wt_t)conv_layer_weights[f][c][m][n];

    // Bias
    ifstream ifs_conv_bias("../bin/conv_bias.bin", ios::in | ios::binary);
    ifs_conv_bias.read((char *)(conv_layer_bias), 64 * sizeof(float));
    ifs_conv_bias.close();

    // Typecast to fixed-point
    for (int f = 0; f < 64; f++)
        fixp_conv_layer_bias[f] = (wt_t)conv_layer_bias[f];

    // Golden Output
    ifstream ifs_golden_output("../bin/conv_output.bin", ios::in | ios::binary);
    ifs_golden_output.read((char *)(**conv_layer_golden_output_feature_map), 64 * 368 * 640 * sizeof(float));
    ifs_golden_output.close();
}

//--------------------------------------------------------------------------
// This is where the real fun begins.
//--------------------------------------------------------------------------
int main()
{
    long double mse = 0.0;
    hls::stream<ap_axis<16, 1, 1, 1>> board_output_feature_map;
    // Read reference inputs, parameters, and output
    read_bin_files();

    std::cout << "Beginning HLS tiled-convolution simulation..." << std::endl;

    tiled_conv(fixp_conv_layer_input_feature_map,
               fixp_conv_layer_weights,
               fixp_conv_layer_bias,
               board_output_feature_map);

    ap_axis<16, 1, 1, 1> tmp;
    // flatten output feature map to 1d

    int llimit = 100;

    int i = 0;
    int j = 10;
    while (true)
    {
        if (board_output_feature_map.empty() && j > 0)
        {
            std::cout << "empty" << std::endl;
            j--;
            if (j == 0)
            {
                break;
            }
            continue;
        }
        else
        {

            tmp = board_output_feature_map.read();
            ap_int<16> raw = tmp.data;
            ap_fixed<16, 3> fp;
            for (int t = 0; t < 16; t++)
            {
                fp[t] = raw[t];
            }

            // std::cout << tmp.data << std::endl;
            data[i] = fp;
            // std::cout << data[i] << std::endl;
            if (llimit > 0)
            {
                // std::cout << "reading No. "<<100 - llimit<<": " << data[i] << std::endl;
                llimit--;
            }
            i++;
            if (i == 64 * 368 * 640)
            {
                break;
            }
        }
    }
    int golden_data_index = 0;
    // unflatten output feature map to 3d
    int count = 200;
    float local_tile[4][23][20];
    for (int ti = 0; ti < 16; ++ti)
    {
        for (int tj = 0; tj < 32; ++tj)
        {
            if(tj == 32) {
                // std::cout << "tile: " << ti << " " << tj << " " << tk << std::endl;
                return 0;
            }
            for (int tk = 0; tk < 16; ++tk)
            {
                // std::cout << "tile: " << ti << " " << tj << " " << tk << std::endl;
                int i_start = ti * 23;
                int j_start = tj * 20;
                int k_start = tk * 4;
                
                // read in local tile
                for (int k = 0; k < 4; k++)
                {
                    for (int i = 0; i < 23; i++)
                    {
                        for (int j = 0; j < 20; j++)
                        {
                            // std::cout << "i: " << i_start + i << " j: " << j_start + j << " k: " << k_start + k << std::endl;
                            // std::cout << "bounds: "<< std::endl;
                            // std::cout << "i: " << 64 << " j: " << 368 << " k: " << 640 << std::endl;
                            local_tile[k][i][j] = conv_layer_golden_output_feature_map[k_start + k][i_start + i][j_start + j];
                            
                        }
                    }
                }

                for (int k = 0; k < 4; k++)
                {
                    for (int i = 0; i < 23; i++)
                    {
                        for (int j = 0; j < 20; j++)
                        {
                            golden_data[golden_data_index] = local_tile[k][i][j];
                            golden_data_index++;
                            // std::cout << "i: " << i_start+i << " j: " << j_start+j << " k: " << k_start+k << " data: " << data[i_start+i][j_start+j][k_start+k] << std::endl;
                        }
                    }
                }
            }
        }
    }

    std::cout << "Tiled-convolution simulation complete!\n"
              << std::endl;

    //     // Compute Mean-Squared-Error
    //     for (int f = 0; f < 64; f++)
    //     {
    //         for (int i = 0; i < 368; i++)
    //         {
    //             for (int j = 0; j < 640; j++)
    //             {
    //                 mse += std::pow((conv_layer_golden_output_feature_map[f][i][j] - (float)fixp_conv_layer_output_feature_map[f][i][j]), 2);
    //             }
    //         }
    // #ifdef PRINT_DEBUG
    //         // Prints sample output values (first feature of each channel) for comparison
    //         // Modify as required for debugging
    //         int row = 367;
    //         int col = 1279;

    //         cout << "Output feature[" << f << "][" << row << "][" << col << "]: ";
    //         cout << "Expected: " << conv_layer_golden_output_feature_map[f][row][col] << "\t";
    //         cout << "Actual: " << fixp_conv_layer_output_feature_map[f][row][col];
    //         cout << endl;
    // #endif
    //     }
    // mse for golden data and data
    for (int i = 0; i < 64 * 368 * 640; i++)
    {
        mse += std::pow((golden_data[i] - (float)data[i]), 2);
    }

    mse = mse / (64 * 368 * 640);

    std::cout << "\nOutput MSE:  " << mse << std::endl;

    std::cout << "----------------------------------------" << std::endl;
#ifdef CSIM_DEBUG
    if (mse > 0 && mse < std::pow(10, -13))
    {
        std::cout << "Floating-point Simulation SUCCESSFUL!!!" << std::endl;
    }
    else
    {
        std::cout << "Floating-point Simulation FAILED :(" << std::endl;
    }
#else
    if (mse > 0 && mse < std::pow(10, -3))
    {
        std::cout << "Fixed-point Simulation SUCCESSFUL!!!" << std::endl;
    }
    else
    {
        std::cout << "Fixed-point Simulation FAILED :(" << std::endl;
    }
#endif
    std::cout << "----------------------------------------" << std::endl;

    return 0;
}
