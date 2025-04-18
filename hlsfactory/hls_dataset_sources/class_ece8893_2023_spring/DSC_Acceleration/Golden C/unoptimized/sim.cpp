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
// Function to read from bin files
//--------------------------------------------------------------------------
template <int out_channel, int inp_channel, int k_height, int k_width>
void read_conv_weight(const string& file,
                      float conv_layer_weights[out_channel][inp_channel][k_height][k_width],
                      wt_t  fixp_conv_layer_weights[out_channel][inp_channel][k_height][k_width]) //TODO : Why does not passing by reference works?
{
    int size = out_channel*inp_channel*k_height*k_width;
    
    ifstream ifs_conv_input(file, ios::in | ios::binary);
    ifs_conv_input.read((char*)(***conv_layer_weights), size*sizeof(float));
    ifs_conv_input.close();

    // Typecast to fixed-point 
    for(int f = 0; f < out_channel; f++)
        for(int c = 0; c < inp_channel; c++)
            for(int m = 0; m < k_height; m++)
                for(int n =0; n < k_width; n++)
                    fixp_conv_layer_weights[f][c][m][n] = (wt_t) conv_layer_weights[f][c][m][n];
}

template <int out_channel>
void read_conv_bias(const string& file,
                    float conv_layer_bias[out_channel],
                    wt_t  fixp_conv_layer_bias[out_channel])
{

    ifstream ifs_conv_bias(file, ios::in | ios::binary);
    ifs_conv_bias.read((char*)(conv_layer_bias), out_channel*sizeof(float));
    ifs_conv_bias.close();

    // Typecast to fixed-point 
    for(int f = 0; f < out_channel; f++)
        fixp_conv_layer_bias[f] = (wt_t) conv_layer_bias[f];
}

//--------------------------------------------------------------------------
// Convolution layer inputs, parameters, and reference output
//--------------------------------------------------------------------------
float conv_layer_input_feature_map[3][96][96];

float conv_layer_weights_0   [32][3][3][3];
float conv_layer_bias_0      [32];
float conv_layer_weights_1_0 [32][1][3][3];
float conv_layer_bias_1_0    [32];
float conv_layer_weights_1_1 [16][32][1][1];
float conv_layer_bias_1_1    [16];
float conv_layer_weights_2_0 [96][16][1][1];
float conv_layer_bias_2_0    [96];
float conv_layer_weights_2_1 [1][96][3][3];
float conv_layer_bias_2_1    [96];
float conv_layer_weights_2_2 [24][96][1][1];
float conv_layer_bias_2_2    [24];

float conv_layer_golden_output_feature_map[24][24][24];


//--------------------------------------------------------------------------
// Convolution layer inputs, parameters for HLS
//--------------------------------------------------------------------------
fm_t  fixp_conv_layer_input_feature_map[3][96][96];

fm_t  fixp_conv_ifmap_0            [32][48][48];
fm_t  fixp_conv_ifmap_1_0          [32][48][48];
fm_t  fixp_conv_ifmap_2_0          [16][48][48];
fm_t  fixp_conv_ifmap_2_1          [96][48][48];
fm_t  fixp_conv_ifmap_2_2          [96][24][24];

wt_t  fixp_conv_layer_weights_0    [32][3][3][3];
wt_t  fixp_conv_layer_bias_0       [32];
wt_t  fixp_conv_layer_weights_1_0  [32][1][3][3];
wt_t  fixp_conv_layer_bias_1_0     [32];
wt_t  fixp_conv_layer_weights_1_1  [16][32][1][1];
wt_t  fixp_conv_layer_bias_1_1     [16];
wt_t  fixp_conv_layer_weights_2_0  [96][16][1][1];
wt_t  fixp_conv_layer_bias_2_0     [96];
wt_t  fixp_conv_layer_weights_2_1  [1][96][3][3];
wt_t  fixp_conv_layer_bias_2_1     [96];
wt_t  fixp_conv_layer_weights_2_2  [24][96][1][1];
wt_t  fixp_conv_layer_bias_2_2     [24];

fm_t  fixp_conv_layer_output_feature_map[24][24][24] = {0};

//--------------------------------------------------------------------------
// Read the reference files into test bench arrays
//--------------------------------------------------------------------------
void read_bin_files()
{
    // Input Feature Map
    ifstream ifs_conv_input("../../DSC/hw/bin/inputs/input.bin", ios::in | ios::binary);
    ifs_conv_input.read((char*)(**conv_layer_input_feature_map), 3*96*96*sizeof(float));
    ifs_conv_input.close();

    // Typecast to fixed-point 
    for(int c = 0; c < 3; c++)
        for(int i = 0; i < 96; i++)
            for(int j = 0; j < 96; j++)
                fixp_conv_layer_input_feature_map[c][i][j] = (fm_t) conv_layer_input_feature_map[c][i][j];    
    
 
    // Weights and biases
    read_conv_weight <32,3,3,3>  ("../../DSC/hw/bin/weights/fused_features_0_conv0_bn0_weights.bin", conv_layer_weights_0, fixp_conv_layer_weights_0);
    read_conv_bias   <32>        ("../../DSC/hw/bin/weights/fused_features_0_conv0_bn0_bias.bin", conv_layer_bias_0, fixp_conv_layer_bias_0);

    read_conv_weight <32,1,3,3>  ("../../DSC/hw/bin/weights/fused_features_1_conv_0_conv0_bn0_weights.bin", conv_layer_weights_1_0, fixp_conv_layer_weights_1_0);
    read_conv_bias   <32>        ("../../DSC/hw/bin/weights/fused_features_1_conv_0_conv0_bn0_bias.bin", conv_layer_bias_1_0, fixp_conv_layer_bias_1_0);
    read_conv_weight <16,32,1,1> ("../../DSC/hw/bin/weights/fused_features_1_conv_conv1_bn1_weights.bin", conv_layer_weights_1_1, fixp_conv_layer_weights_1_1);
    read_conv_bias   <16>        ("../../DSC/hw/bin/weights/fused_features_1_conv_conv1_bn1_bias.bin", conv_layer_bias_1_1, fixp_conv_layer_bias_1_1);

    read_conv_weight <96,16,1,1> ("../../DSC/hw/bin/weights/fused_features_2_conv_0_conv0_bn0_weights.bin", conv_layer_weights_2_0, fixp_conv_layer_weights_2_0);
    read_conv_bias   <96>        ("../../DSC/hw/bin/weights/fused_features_2_conv_0_conv0_bn0_bias.bin", conv_layer_bias_2_0, fixp_conv_layer_bias_2_0);
    read_conv_weight <1,96,3,3> ("../../DSC/hw/bin/weights/fused_features_2_conv_1_conv0_bn0_weights.bin", conv_layer_weights_2_1, fixp_conv_layer_weights_2_1);
    read_conv_bias   <96>        ("../../DSC/hw/bin/weights/fused_features_2_conv_1_conv0_bn0_bias.bin", conv_layer_bias_2_1, fixp_conv_layer_bias_2_1);
    read_conv_weight <24,96,1,1> ("../../DSC/hw/bin/weights/fused_features_2_conv_conv2_bn2_weights.bin", conv_layer_weights_2_2, fixp_conv_layer_weights_2_2);
    read_conv_bias   <24>        ("../../DSC/hw/bin/weights/fused_features_2_conv_conv2_bn2_bias.bin", conv_layer_bias_2_2, fixp_conv_layer_bias_2_2);


    // Golden Output
    ifstream ifs_golden_output("../../DSC/hw/bin/outputs/features_2_conv_3.bin", ios::in | ios::binary);
    ifs_golden_output.read((char*)(**conv_layer_golden_output_feature_map), 24*24*24*sizeof(float));    
    ifs_golden_output.close();
}

//--------------------------------------------------------------------------
// This is where the real fun begins.
//--------------------------------------------------------------------------
int main ()
{
    long double mse = 0.0;
    
    // Read reference inputs, parameters, and output
    read_bin_files();
   
    std::cout << "Beginning HLS tiled-convolution simulation..." << std::endl;
    
    tiled_conv (fixp_conv_layer_input_feature_map,

                fixp_conv_ifmap_0,
                fixp_conv_ifmap_1_0,
                fixp_conv_ifmap_2_0,
                fixp_conv_ifmap_2_1,
                fixp_conv_ifmap_2_2,

                fixp_conv_layer_weights_0,
                fixp_conv_layer_bias_0,
                fixp_conv_layer_weights_1_0,
                fixp_conv_layer_bias_1_0,
                fixp_conv_layer_weights_1_1,
                fixp_conv_layer_bias_1_1,
                fixp_conv_layer_weights_2_0,
                fixp_conv_layer_bias_2_0,
                fixp_conv_layer_weights_2_1,
                fixp_conv_layer_bias_2_1,
                fixp_conv_layer_weights_2_2,
                fixp_conv_layer_bias_2_2,

                fixp_conv_layer_output_feature_map
    );
    
    std::cout << "Tiled-convolution simulation complete!\n" << std::endl;
    
    //Compute Mean-Squared-Error
    for(int f = 0; f < 24; f++)
    {
        for(int i = 0; i < 24; i++)
        {
            for(int j = 0; j < 24; j++)
            {
                mse += std::pow((conv_layer_golden_output_feature_map[f][i][j] 
                                 -(float) fixp_conv_layer_output_feature_map[f][i][j]), 2);
        //    }
        //}
        //#ifdef PRINT_DEBUG
        if ((conv_layer_golden_output_feature_map[f][i][j] - fixp_conv_layer_output_feature_map[f][i][j]) > 0.01) {
            // Prints sample output values (first feature of each channel) for comparison
            // Modify as required for debugging
            int row = i;
            int col = j;
            
            cout << "Output feature[" << f << "][" << row << "][" << col << "]: ";
            cout << "Expected: " << conv_layer_golden_output_feature_map[f][row][col] << "\t"; 
            cout << "Actual: "   << fixp_conv_layer_output_feature_map[f][row][col];
            cout << endl;
        }
        //#endif
            }
        }     
    }
    
    mse = mse / (24 * 24 * 24);

    std::cout << "\nOutput MSE:  " << mse << std::endl;
    
    std::cout << "----------------------------------------" << std::endl;
    #ifdef CSIM_DEBUG
        if((mse > 0 && mse < std::pow(10,-10)) or mse == 0)
        {
            std::cout << "Floating-point Simulation SUCCESSFUL!!!" << std::endl;
        }
        else
        {
            std::cout << "Floating-point Simulation FAILED :(" << std::endl;
        }
    #else
        if(mse > 0 && mse < std::pow(10,-3))
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
