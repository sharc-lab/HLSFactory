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
float conv_layer_input_feature_map[64][104][104];
float conv_layer_weights_l1[64][64][3][3];
float conv_layer_bias_l1[64];
float conv_layer_weights_l2[32][32][3][3];
float conv_layer_bias_l2[32];
float conv_layer_golden_output_L2_feature_map[32][104][104];
float conv_layer_golden_output_L1_feature_map[64][104][104];

fm_t  fixp_conv_layer_input[64][104][104];
fm_t  fixp_conv_layer_input_feature_map[1024][416][416];
wt_t  fixp_conv_layer_weights_l1[64][1024][3][3];
wt_t  fixp_conv_layer_bias_l1[1024];

wt_t  fixp_conv_layer_weights_l2[32][1024][3][3];
wt_t  fixp_conv_layer_bias_l2[1024];

fm_t  fixp_conv_layer_output_feature_map[1024][416][416] = {0};
fm_t  fixp_conv_layer_output_L1_feature_map[64][104][104] = {0};
fm_t  fixp_conv_layer_output_L2_feature_map[32][104][104] = {0};

//--------------------------------------------------------------------------
// Read the reference files into test bench arrays
//--------------------------------------------------------------------------
void read_bin_files()
{
    // Input Feature Map
    ifstream ifs_conv_input("./conv_layer1_input.bin", ios::in | ios::binary);
    ifs_conv_input.read((char*)(**conv_layer_input_feature_map), 64*104*104*sizeof(float));
    ifs_conv_input.close();

    // Typecast to fixed-point 
    for(int c = 0; c < 64; c++)
        for(int i = 0; i < 104; i++)
            for(int j = 0; j < 104; j++)
                fixp_conv_layer_input[c][i][j] = (fm_t) conv_layer_input_feature_map[c][i][j];    
    
    // Weights
    ifstream ifs_conv_weights1("./conv_layer1_weights.bin", ios::in | ios::binary);
    ifs_conv_weights1.read((char*)(***conv_layer_weights_l1), 64*64*3*3*sizeof(float));
    ifs_conv_weights1.close();
    
    // Typecast to fixed-point 
    for(int f = 0; f < 64; f++)
        for(int c = 0; c < 64; c++)
            for(int m = 0; m < 3; m++)
                for(int n =0; n < 3; n++)
                    fixp_conv_layer_weights_l1[f][c][m][n] = (wt_t) conv_layer_weights_l1[f][c][m][n];
    
    // Bias
    ifstream ifs_conv_bias1("./conv_layer1_bias.bin", ios::in | ios::binary);
    ifs_conv_bias1.read((char*)(conv_layer_bias_l1), 64*sizeof(float));
    ifs_conv_bias1.close();
    
    // Typecast to fixed-point 
    for(int f = 0; f < 64; f++)
        fixp_conv_layer_bias_l1[f] = (wt_t) conv_layer_bias_l1[f];

    // Weights
    ifstream ifs_conv_weights2("./conv_layer2_weights.bin", ios::in | ios::binary);
    ifs_conv_weights2.read((char*)(***conv_layer_weights_l2), 32*32*3*3*sizeof(float));
    ifs_conv_weights2.close();
    
    // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        for(int c = 0; c < 32; c++)
            for(int m = 0; m < 3; m++)
                for(int n =0; n < 3; n++)
                    fixp_conv_layer_weights_l2[f][c][m][n] = (wt_t) conv_layer_weights_l2[f][c][m][n];
    
    // Bias
    ifstream ifs_conv_bias2("./conv_layer2_bias.bin", ios::in | ios::binary);
    ifs_conv_bias2.read((char*)(conv_layer_bias_l2), 32*sizeof(float));
    ifs_conv_bias2.close();
    
    // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        fixp_conv_layer_bias_l2[f] = (wt_t) conv_layer_bias_l2[f];




    // Golden Output
    ifstream ifs_golden_output1("./conv_layer1_output.bin", ios::in | ios::binary);
    ifs_golden_output1.read((char*)(**conv_layer_golden_output_L1_feature_map), 64*104*104*sizeof(float));    
    ifs_golden_output1.close();

    ifstream ifs_golden_output2("./conv_layer2_output.bin", ios::in | ios::binary);
    ifs_golden_output2.read((char*)(**conv_layer_golden_output_L2_feature_map), 32*104*104*sizeof(float));    
    ifs_golden_output2.close();

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
    
    yolov4_tiny (fixp_conv_layer_input,
                fixp_conv_layer_weights_l1,
                fixp_conv_layer_bias_l1,
                fixp_conv_layer_weights_l2,
                fixp_conv_layer_bias_l2,
                fixp_conv_layer_output_feature_map,
		fixp_conv_layer_output_L1_feature_map,
		fixp_conv_layer_output_L2_feature_map,
		fixp_conv_layer_input_feature_map
    );
    
    std::cout << "Tiled-convolution simulation complete!\n" << std::endl;
    
    //Compute Mean-Squared-Error
    for(int f = 0; f < 64; f++)
    {
        for(int i = 0; i < 104; i++)
        {
            for(int j = 0; j < 104; j++)
            {
                mse += std::pow((conv_layer_golden_output_L1_feature_map[f][i][j] 
                                 -(float) fixp_conv_layer_output_L1_feature_map[f][i][j]), 2);

            }
        }
	#ifdef PRINT_DEBUG
            // Prints sample output values (first feature of each channel) for comparison
            // Modify as required for debugging
            int row = 0;
            int col = 0;

            cout << "Output feature[" << f << "][" << row << "][" << col << "]: ";
            cout << "Expected: " << conv_layer_golden_output_feature_map[f][row][col] << "\t"; 
            cout << "Actual: "   << fixp_conv_layer_output_L12_feature_map[f][row][col];
            cout << endl;
	    cout << "Next feature map:" << endl; 
        #endif        
    }
    
    mse = mse / (64 * 104 * 104);

    std::cout << "\nOutput MSE - 1:  " << mse << std::endl;
  
   mse = 0.0; 

    for(int f = 0; f < 32; f++)
    {
        for(int i = 0; i < 104; i++)
        {
            for(int j = 0; j < 104; j++)
            {
                mse += std::pow((conv_layer_golden_output_L1_feature_map[f][i][j] 
                                 -(float) fixp_conv_layer_output_L1_feature_map[f][i][j]), 2);

            }
        }
	#ifdef PRINT_DEBUG
            // Prints sample output values (first feature of each channel) for comparison
            // Modify as required for debugging
            int row = 0;
            int col = 0;

            cout << "Output feature[" << f << "][" << row << "][" << col << "]: ";
            cout << "Expected: " << conv_layer_golden_output_L2_feature_map[f][row][col] << "\t"; 
            cout << "Actual: "   << fixp_conv_layer_output_L2_feature_map[f][row][col];
            cout << endl;
	    cout << "Next feature map:" << endl; 
        #endif        
    }
    
    mse = mse / (32 * 104 * 104);

    std::cout << "\nOutput MSE - 2:  " << mse << std::endl;







    std::cout << "----------------------------------------" << std::endl;
    #ifdef CSIM_DEBUG
        if(mse > 0 && mse < std::pow(10,-12))
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
