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
float conv_layer_input_feature_map[128][104][104];
float conv_layer_golden_output_feature_map[128][52][52];

fm_t  fixp_conv_layer_input_feature_map[128][104][104];
fm_t  fixp_conv_layer_output_feature_map[128][52][52] = {0};

//--------------------------------------------------------------------------
// Read the reference files into test bench arrays
//--------------------------------------------------------------------------
void read_bin_files()
{
    // Input Feature Map
    ifstream ifs_conv_input("./bin/maxpool_input.bin", ios::in | ios::binary);
    ifs_conv_input.read((char*)(**conv_layer_input_feature_map), 128*104*104*sizeof(float));
    ifs_conv_input.close();

    // Typecast to fixed-point 
    for(int c = 0; c < 128; c++)
        for(int i = 0; i < 104; i++)
            for(int j = 0; j < 104; j++)
                fixp_conv_layer_input_feature_map[c][i][j] = (fm_t) conv_layer_input_feature_map[c][i][j];    
    
    
    // Golden Output
    ifstream ifs_golden_output("./bin/maxpool_output.bin", ios::in | ios::binary);
    ifs_golden_output.read((char*)(**conv_layer_golden_output_feature_map), 128*52*52*sizeof(float));    
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
    
    tiled_maxpool2D (fixp_conv_layer_input_feature_map,
                fixp_conv_layer_output_feature_map
    );
    
    std::cout << "Tiled-convolution simulation complete!\n" << std::endl;
    
    //Compute Mean-Squared-Error
    for(int f = 0; f < 128; f++)
    {
        for(int i = 0; i < 52; i++)
        {
            for(int j = 0; j < 52; j++)
            {
                mse += std::pow((conv_layer_golden_output_feature_map[f][i][j] 
                                 -(float) fixp_conv_layer_output_feature_map[f][i][j]), 2);
            }
        }
	#ifdef PRINT_DEBUG
            // Prints sample output values (first feature of each channel) for comparison
            // Modify as required for debugging
            int row = 0;
            int col = 0;

            cout << "Output feature[" << f << "][" << row << "][" << col << "]: ";
            cout << "Expected: " << conv_layer_golden_output_feature_map[f][row][col] << "\t";
            cout << "Actual: "   << fixp_conv_layer_output_feature_map[f][row][col];
            cout << endl;
            cout << "Next feature map:" << endl;

        #endif        
    }
    
    mse = mse / (32 * 104 * 104);

    std::cout << "\nOutput MSE:  " << mse << std::endl;
    
    std::cout << "----------------------------------------" << std::endl;
    #ifdef CSIM_DEBUG
        if(mse == 0)
        {
            std::cout << "Floating-point Simulation SUCCESSFUL!!!" << std::endl;
        }
        else
        {
            std::cout << "Floating-point Simulation FAILED :(" << std::endl;
        }
    #else
        if(mse ==  0)
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
