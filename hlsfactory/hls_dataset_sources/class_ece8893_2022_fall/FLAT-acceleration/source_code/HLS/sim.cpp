//---------------------------------------------
// C++ Test bench for FLAT
//---------------------------------------------
#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>

#include "flat.h"

using namespace std;

//---------------------------------------------
// Input Declaration
//---------------------------------------------
float attention_layer_query_input[576][64][16][64];
float attention_layer_key_input[576][64][16][64];
float attention_layer_value_input[576][64][16][64];
float attention_layer_bias_input[64][16][64][64];

data_t fixp_attention_layer_query_input[576][64][16][64];
data_t fixp_attention_layer_key_input[576][64][16][64];
data_t fixp_attention_layer_value_input[576][64][16][64];
data_t fixp_attention_layer_bias_input[64][16][64][64];

//---------------------------------------------
// Output Declaration
//---------------------------------------------
float attention_out[576][64][16][64];
data_t fixp_attention_out[576][64][16][64]={0};

//---------------------------------------------
// Read the reference files into test bench arrays
//---------------------------------------------
void read_bin_files()
{
    //Query Feature Map
    ifstream ifs_query("./bin/eight_query.bin", ios::in|ios::binary);
    ifs_query.read((char*)(***attention_layer_query_input), 576*64*16*64*sizeof(float));
    ifs_query.close();

    //Key Feature Map
    ifstream ifs_key("./bin/eight_key.bin", ios::in|ios::binary);
    ifs_key.read((char*)(***attention_layer_key_input), 576*64*16*64*sizeof(float));
    ifs_key.close();

    //Value Feature Map
    ifstream ifs_value("./bin/eight_value.bin", ios::in|ios::binary);
    ifs_value.read((char*)(***attention_layer_value_input), 576*64*16*64*sizeof(float));
    ifs_value.close();

    //Bias Feature Map
    ifstream ifs_bias("./bin/eight_bias.bin", ios::in|ios::binary);
    ifs_bias.read((char*)(***attention_layer_bias_input), 64*16*64*64*sizeof(float));
    ifs_bias.close();

    //Golden Output
    ifstream ifs_target_output("./bin/eight_golden_output_final.bin", ios::in|ios::binary);
    ifs_target_output.read((char*)(***attention_out), 576*64*16*64*sizeof(float));
    ifs_target_output.close();
}

//--------------------------------------------------------------------------
// Convert the data types of every array element for specified 
// configuration.
//--------------------------------------------------------------------------
void convert_type()
{
    //Convert data type for key
    for (int b = 0; b < 576; ++b)
    {
        for (int t = 0; t < 64; ++t)
        {
            for (int n = 0; n < 16; ++n)
            {
                for (int h = 0; h < 64; ++h)
                {
                    //std::cout << attention_layer_key_input[b][t][n][h] << std::endl;
                    fixp_attention_layer_value_input[b][t][n][h] = (data_t)attention_layer_value_input[b][t][n][h];
                    fixp_attention_layer_key_input[b][t][n][h] = (data_t)attention_layer_key_input[b][t][n][h];
                }
            }
        }
    }

    //Convert data type for value
    for (int b = 0; b < 576; ++b)
    {
        for (int f = 0; f < 64; ++f)
        {
            for (int n = 0; n < 16; ++n)
            {
                for (int h = 0; h < 64; ++h)
                {
                    fixp_attention_layer_query_input[b][f][n][h] = (data_t)attention_layer_query_input[b][f][n][h];
                    //fixp_attention_out[b][f][n][h] = (data_t)attention_out[b][f][n][h];
                    //std::cout << fixp_attention_out[b][f][n][h] << std::endl;
                }
            }
        }
    }

    for (int b = 0; b < 64; ++b)
    {
        for (int n = 0; n < 16; ++n)
        {
            for (int f = 0; f < 64; ++f)
            {
                for (int t = 0; t < 64; ++t)
                {
                    fixp_attention_layer_bias_input[b][n][f][t] = (data_t)attention_layer_bias_input[b][n][f][t];
                }
            }
        }
    }
}

//Test Correctness of FLAT
int main()
{
    long double mse=0.0;

    read_bin_files();

    convert_type();
    
    #ifdef CMODEL_SIM
        std::cout << "Beginning C model simulation..." << std::endl;
        FlatDataflow (fixp_attention_layer_query_input,
                    fixp_attention_layer_key_input,
                    fixp_attention_layer_value_input,
                    fixp_attention_layer_bias_input,
                    fixp_attention_out
        );
        std::cout << "C model simulation complete!\n" << std::endl;
    #else
        std::cout << "Beginning HLS tiled-convolution simulation..." << std::endl;
        FlatDataflow (fixp_attention_layer_query_input,
                    fixp_attention_layer_key_input,
                    fixp_attention_layer_value_input,
                    fixp_attention_layer_bias_input,
                    fixp_attention_out
        );
        std::cout << "Tiled-convolution simulation complete!\n" << std::endl;
    #endif

    //Compute MSE
    for (int b = 0; b < 576; ++b)
    {
        for (int f = 0; f < 64; ++f)
        {
            for (int n = 0; n < 16; ++n)
            {
                for (int h = 0; h < 64; ++h)
                {
                    #ifdef CMODEL_SIM
                        mse += std::pow((attention_out[b][f][n][h]
                              - (float) fixp_attention_out[b][f][n][h]), 2);
                    #else 
                        mse = 0;
                    #endif
                }
            }
        }
    }
    
    mse = mse / (576 * 64 * 64 * 16);

    std::cout << "Output MSE:  " << mse << std::endl;
    
    std::cout << "----------------------------------------" << std::endl;
    #ifdef CSIM_DEBUG
        if(mse > 0 && mse < std::exp(-13))
        {
            std::cout << "Simulation SUCCESSFUL!!!" << std::endl;
        }
        else
        {
            std::cout << "Simulation FAILED :(" << std::endl;
        }
    #else
        if(mse > 0 && mse < std::exp(-3))
        {
            std::cout << "Simulation SUCCESSFUL!!!" << std::endl;
        }
        else
        {
            std::cout << "Simulation FAILED :(" << std::endl;
        }
    #endif
    std::cout << "----------------------------------------" << std::endl;

    return 0;
}
