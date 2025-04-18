//--------------------------------------------------------------------------
// ECE8893 - PARALLEL PROGRAMMING FOR FPGAs - FINAL PROJECT
//
// Test bench for Yolov3-tiny.
//
//--------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <cmath>

#include "conv.h"

using namespace std;

//#define CMODEL_SIM //temp - to be removed
float bn1_max, bn1_min;

//--------------------------------------------------------------------------
// Convolution layer inputs and reference output
//--------------------------------------------------------------------------
float input_image[3][416][416];
//float   conv_layer_golden_output_feature_map[64][184][320];

//--------------------------------------
//Weights for various convolution layers
//--------------------------------------
float conv_layer_1_weights[16][3][3][3];
float conv_layer_2_weights[32][16][3][3];
float conv_layer_3_weights[64][32][3][3];
float conv_layer_4_weights[128][64][3][3];
float conv_layer_5_weights[256][128][3][3];
float conv_layer_6_weights[512][256][3][3];
float conv_layer_7_weights[1024][512][3][3];
float conv_layer_8_weights[256][1024][1][1];
float conv_layer_9_weights[512][256][3][3];
float conv_layer_10_weights[255][512][1][1];
float conv_layer_11_weights[128][256][1][1];
float conv_layer_12_weights[256][384][3][3];
float conv_layer_13_weights[255][256][1][1];

//----------------------------------------------
//Weights for various batch normalization layers
//----------------------------------------------
float bn_layer_1_weights[16][4];
float bn_layer_2_weights[32][4];
float bn_layer_3_weights[64][4];
float bn_layer_4_weights[128][4];
float bn_layer_5_weights[256][4];
float bn_layer_6_weights[512][4];
float bn_layer_7_weights[1024][4];
float bn_layer_8_weights[256][4];
float bn_layer_9_weights[512][4];
float bn_layer_11_weights[128][4];
float bn_layer_12_weights[256][4];

//----
//Bias
//----
float bias_layer_10[255];
float bias_layer_13[255];

fm_t    fixp_conv_layer_input_feature_map[1024][416][416];
fm_t    fixp_input_image[3][416][416];

wt_t    fixp_conv_layer_1_weights[16][1024][3][3];
wt_t    fixp_conv_layer_2_weights[32][1024][3][3];
wt_t    fixp_conv_layer_3_weights[64][1024][3][3];
wt_t    fixp_conv_layer_4_weights[128][1024][3][3];
wt_t    fixp_conv_layer_5_weights[256][1024][3][3];
wt_t    fixp_conv_layer_6_weights[512][1024][3][3];
wt_t    fixp_conv_layer_7_weights[1024][1024][3][3];
wt_t    fixp_conv_layer_8_weights[256][1024][1][1];
wt_t    fixp_conv_layer_9_weights[512][1024][3][3];
wt_t    fixp_conv_layer_10_weights[255][1024][1][1];
wt_t    fixp_conv_layer_11_weights[128][1024][1][1];
wt_t    fixp_conv_layer_12_weights[256][1024][3][3];
wt_t    fixp_conv_layer_13_weights[255][1024][1][1];

wt_t    fixp_bn_layer_1_weights[1024][4];
wt_t    fixp_bn_layer_2_weights[1024][4];
wt_t    fixp_bn_layer_3_weights[1024][4];
wt_t    fixp_bn_layer_4_weights[128][4];
wt_t    fixp_bn_layer_5_weights[256][4];
wt_t    fixp_bn_layer_6_weights[512][4];
wt_t    fixp_bn_layer_7_weights[1024][4];
wt_t    fixp_bn_layer_8_weights[256][4];
wt_t    fixp_bn_layer_9_weights[512][4];
wt_t    fixp_bn_layer_11_weights[128][4];
wt_t    fixp_bn_layer_12_weights[256][4];

wt_t    fixp_bias_layer_10[255];
wt_t    fixp_bias_layer_13[255];

wt_t    fixp_yolo_layer_16_stride = 16;
wt_t    fixp_yolo_layer_23_stride = 32;
wt_t    fixp_yolo_layer_16_anchor[3][2] = {{10, 14},  {23, 27},   {37, 58}};
wt_t    fixp_yolo_layer_23_anchor[3][2] = {{81,  82}, {135, 169}, {344, 319}};


//--------------------------------------------------------------------------
// Computed outputs
//--------------------------------------------------------------------------
fm_t   fixp_conv_layer_output_feature_map[1024][416][416]   = {0};
fm_t   fixp_conv_layer_13_output_feature_map[256][416][416] = {0};
fm_t   fixp_conv_layer_8_output_feature_map[256][416][416]  = {0};

void print_layers(
    fm_t *input_feature_map,
    fm_t *output_feature_map,
    int input_d,
    int input_h,
    int input_w
)
{
    for(int f=0; f < input_d; ++f)
        {
            std::cout << "Printing for layer " << f << std::endl;
            std::cout << "Layer" << "\t i " << "\t j " << "\t Input value" << "\t Output value" << std::endl;
            for(int i=0; i < input_h; ++i)
                for(int j=0; j < input_w; ++j)
                {
                    //std::cout << "i = " << i << " j = " << j << "\t \t";
                    std::cout << f << "\t" << i << "\t" << j << "\t" << input_feature_map[(f*input_h*input_w)+(i*input_w)+(j)] << "\t" << output_feature_map[(f*input_h*input_w)+(i*input_w)+(j)] << std::endl;
                }
        }
}

//--------------------------------------------------------------------------
// Read the reference files into test bench arrays
//--------------------------------------------------------------------------
void read_bin_files()
{
    // Image input
    ifstream ifs_image_input("../bin/img/input_img_float.bin", ios::in | ios::binary);
    ifs_image_input.read((char*)(**input_image), 3*416*416*sizeof(float));
    ifs_image_input.close();

    //Convolution layer weights
    ifstream ifs_conv_weights_1("../bin/conv/conv_weights_1.bin", ios::in | ios::binary);
    ifs_conv_weights_1.read((char*)(***conv_layer_1_weights), 16*3*3*3*sizeof(float));
    ifs_conv_weights_1.close();

    ifstream ifs_conv_weights_2("../bin/conv/conv_weights_2.bin", ios::in | ios::binary);
    ifs_conv_weights_2.read((char*)(***conv_layer_2_weights), 32*16*3*3*sizeof(float));
    ifs_conv_weights_2.close();

    ifstream ifs_conv_weights_3("../bin/conv/conv_weights_3.bin", ios::in | ios::binary);
    ifs_conv_weights_3.read((char*)(***conv_layer_3_weights), 64*32*3*3*sizeof(float));
    ifs_conv_weights_3.close();

    ifstream ifs_conv_weights_4("../bin/conv/conv_weights_4.bin", ios::in | ios::binary);
    ifs_conv_weights_4.read((char*)(***conv_layer_4_weights), 128*64*3*3*sizeof(float));
    ifs_conv_weights_4.close();

    ifstream ifs_conv_weights_5("../bin/conv/conv_weights_5.bin", ios::in | ios::binary);
    ifs_conv_weights_5.read((char*)(***conv_layer_5_weights), 256*128*3*3*sizeof(float));
    ifs_conv_weights_5.close();

    ifstream ifs_conv_weights_6("../bin/conv/conv_weights_6.bin", ios::in | ios::binary);
    ifs_conv_weights_6.read((char*)(***conv_layer_6_weights), 512*256*3*3*sizeof(float));
    ifs_conv_weights_6.close();

    ifstream ifs_conv_weights_7("../bin/conv/conv_weights_7.bin", ios::in | ios::binary);
    ifs_conv_weights_7.read((char*)(***conv_layer_7_weights), 1024*512*3*3*sizeof(float));
    ifs_conv_weights_7.close();

    ifstream ifs_conv_weights_8("../bin/conv/conv_weights_8.bin", ios::in | ios::binary);
    ifs_conv_weights_8.read((char*)(***conv_layer_8_weights), 256*1024*1*1*sizeof(float));
    ifs_conv_weights_8.close();

    ifstream ifs_conv_weights_9("../bin/conv/conv_weights_9.bin", ios::in | ios::binary);
    ifs_conv_weights_9.read((char*)(***conv_layer_9_weights), 512*256*3*3*sizeof(float));
    ifs_conv_weights_9.close();

    ifstream ifs_conv_weights_10("../bin/conv/conv_weights_10.bin", ios::in | ios::binary);
    ifs_conv_weights_10.read((char*)(***conv_layer_10_weights), 255*512*1*1*sizeof(float));
    ifs_conv_weights_10.close();

    ifstream ifs_conv_weights_11("../bin/conv/conv_weights_11.bin", ios::in | ios::binary);
    ifs_conv_weights_11.read((char*)(***conv_layer_11_weights), 128*256*1*1*sizeof(float));
    ifs_conv_weights_11.close();

    ifstream ifs_conv_weights_12("../bin/conv/conv_weights_12.bin", ios::in | ios::binary);
    ifs_conv_weights_12.read((char*)(***conv_layer_12_weights), 256*384*3*3*sizeof(float));
    ifs_conv_weights_12.close();

    ifstream ifs_conv_weights_13("../bin/conv/conv_weights_13.bin", ios::in | ios::binary);
    ifs_conv_weights_13.read((char*)(***conv_layer_13_weights), 255*256*1*1*sizeof(float));
    ifs_conv_weights_13.close();

    //Batch Normalisation layer weights
    ifstream ifs_bn_weights_1("../bin/bn/bn_weights_1.bin", ios::in | ios::binary);
    ifs_bn_weights_1.read((char*)(*bn_layer_1_weights), 16*4*sizeof(float));
    ifs_bn_weights_1.close();

    ifstream ifs_bn_weights_2("../bin/bn/bn_weights_2.bin", ios::in | ios::binary);
    ifs_bn_weights_2.read((char*)(*bn_layer_2_weights), 32*4*sizeof(float));
    ifs_bn_weights_2.close();

    ifstream ifs_bn_weights_3("../bin/bn/bn_weights_3.bin", ios::in | ios::binary);
    ifs_bn_weights_3.read((char*)(*bn_layer_3_weights), 64*4*sizeof(float));
    ifs_bn_weights_3.close();

    ifstream ifs_bn_weights_4("../bin/bn/bn_weights_4.bin", ios::in | ios::binary);
    ifs_bn_weights_4.read((char*)(*bn_layer_4_weights), 128*4*sizeof(float));
    ifs_bn_weights_4.close();

    ifstream ifs_bn_weights_5("../bin/bn/bn_weights_5.bin", ios::in | ios::binary);
    ifs_bn_weights_5.read((char*)(*bn_layer_5_weights), 256*4*sizeof(float));
    ifs_bn_weights_5.close();

    ifstream ifs_bn_weights_6("../bin/bn/bn_weights_6.bin", ios::in | ios::binary);
    ifs_bn_weights_6.read((char*)(*bn_layer_6_weights), 512*4*sizeof(float));
    ifs_bn_weights_6.close();

    ifstream ifs_bn_weights_7("../bin/bn/bn_weights_7.bin", ios::in | ios::binary);
    ifs_bn_weights_7.read((char*)(*bn_layer_7_weights), 1024*4*sizeof(float));
    ifs_bn_weights_7.close();

    ifstream ifs_bn_weights_8("../bin/bn/bn_weights_8.bin", ios::in | ios::binary);
    ifs_bn_weights_8.read((char*)(*bn_layer_8_weights), 256*4*sizeof(float));
    ifs_bn_weights_8.close();

    ifstream ifs_bn_weights_9("../bin/bn/bn_weights_9.bin", ios::in | ios::binary);
    ifs_bn_weights_9.read((char*)(*bn_layer_9_weights), 512*4*sizeof(float));
    ifs_bn_weights_9.close();

    ifstream ifs_bn_weights_11("../bin/bn/bn_weights_11.bin", ios::in | ios::binary);
    ifs_bn_weights_11.read((char*)(*bn_layer_11_weights), 128*4*sizeof(float));
    ifs_bn_weights_11.close();

    ifstream ifs_bn_weights_12("../bin/bn/bn_weights_12.bin", ios::in | ios::binary);
    ifs_bn_weights_12.read((char*)(*bn_layer_12_weights), 256*4*sizeof(float));
    ifs_bn_weights_12.close();

    //Bias
    ifstream ifs_bias_10("../bin/bias/bias_10.bin", ios::in | ios::binary);
    ifs_bias_10.read((char*)bias_layer_10, 255*sizeof(float));
    ifs_bias_10.close();

    ifstream ifs_bias_13("../bin/bias/bias_13.bin", ios::in | ios::binary);
    ifs_bias_13.read((char*)bias_layer_13, 255*sizeof(float));
    ifs_bias_13.close();

    // Golden Output
    // ifstream ifs_golden_output("../bin/conv_layer_output_feature_map.bin", ios::in | ios::binary);
    // ifs_golden_output.read((char*)(**conv_layer_golden_output_feature_map), 64*184*320*sizeof(float));    
    // ifs_golden_output.close();
}

//--------------------------------------------------------------------------
// Convert the data types of every array element for specified 
// configuration.
//--------------------------------------------------------------------------
void convert_type()
{    
    // Input Image
    for(int c = 0; c < 3; c++)
        for(int i = 0; i < 416; i++)
            for(int j = 0; j < 416; j++)
                fixp_input_image[c][i][j] = (fm_t) input_image[c][i][j];
    
    // Weights for convolution layer
    for(int f = 0; f < 16; f++)
        for(int c = 0; c < 3; c++)
            for(int m = 0; m < 3; m++)
                for(int n = 0; n < 3; n++)
                    fixp_conv_layer_1_weights[f][c][m][n] = (wt_t) conv_layer_1_weights[f][c][m][n];
    
    for(int f = 0; f < 32; f++)
        for(int c = 0; c < 16; c++)
            for(int m = 0; m < 3; m++)
                for(int n = 0; n < 3; n++)
                    fixp_conv_layer_2_weights[f][c][m][n] = (wt_t) conv_layer_2_weights[f][c][m][n];

    for(int f = 0; f < 64; f++)
        for(int c = 0; c < 32; c++)
            for(int m = 0; m < 3; m++)
                for(int n = 0; n < 3; n++)
                    fixp_conv_layer_3_weights[f][c][m][n] = (wt_t) conv_layer_3_weights[f][c][m][n];

    for(int f = 0; f < 128; f++)
        for(int c = 0; c < 64; c++)
            for(int m = 0; m < 3; m++)
                for(int n = 0; n < 3; n++)
                    fixp_conv_layer_4_weights[f][c][m][n] = (wt_t) conv_layer_4_weights[f][c][m][n];

    for(int f = 0; f < 256; f++)
        for(int c = 0; c < 128; c++)
            for(int m = 0; m < 3; m++)
                for(int n = 0; n < 3; n++)
                    fixp_conv_layer_5_weights[f][c][m][n] = (wt_t) conv_layer_5_weights[f][c][m][n];

    for(int f = 0; f < 512; f++)
        for(int c = 0; c < 256; c++)
            for(int m = 0; m < 3; m++)
                for(int n = 0; n < 3; n++)
                    fixp_conv_layer_6_weights[f][c][m][n] = (wt_t) conv_layer_6_weights[f][c][m][n];
    
    for(int f = 0; f < 1024; f++)
        for(int c = 0; c < 512; c++)
            for(int m = 0; m < 3; m++)
                for(int n = 0; n < 3; n++)
                    fixp_conv_layer_7_weights[f][c][m][n] = (wt_t) conv_layer_7_weights[f][c][m][n];
    
    for(int f = 0; f < 256; f++)
        for(int c = 0; c < 1024; c++)
            for(int m = 0; m < 1; m++)
                for(int n = 0; n < 1; n++)
                    fixp_conv_layer_8_weights[f][c][m][n] = (wt_t) conv_layer_8_weights[f][c][m][n];

    for(int f = 0; f < 512; f++)
        for(int c = 0; c < 256; c++)
            for(int m = 0; m < 3; m++)
                for(int n = 0; n < 3; n++)
                    fixp_conv_layer_9_weights[f][c][m][n] = (wt_t) conv_layer_9_weights[f][c][m][n];

    for(int f = 0; f < 255; f++)
        for(int c = 0; c < 512; c++)
            for(int m = 0; m < 1; m++)
                for(int n = 0; n < 1; n++)
                    fixp_conv_layer_10_weights[f][c][m][n] = (wt_t) conv_layer_10_weights[f][c][m][n];

    for(int f = 0; f < 128; f++)
        for(int c = 0; c < 256; c++)
            for(int m = 0; m < 1; m++)
                for(int n = 0; n < 1; n++)
                    fixp_conv_layer_11_weights[f][c][m][n] = (wt_t) conv_layer_11_weights[f][c][m][n];

    for(int f = 0; f < 256; f++)
        for(int c = 0; c < 384; c++)
            for(int m = 0; m < 3; m++)
                for(int n = 0; n < 3; n++)
                    fixp_conv_layer_12_weights[f][c][m][n] = (wt_t) conv_layer_12_weights[f][c][m][n];
    
    for(int f = 0; f < 255; f++)
        for(int c = 0; c < 256; c++)
            for(int m = 0; m < 1; m++)
                for(int n = 0; n < 1; n++)
                    fixp_conv_layer_13_weights[f][c][m][n] = (wt_t) conv_layer_13_weights[f][c][m][n];
      
    // Weights for batch normalization layer
    bn1_max = bn_layer_1_weights[0][0];
    bn1_min = bn_layer_1_weights[0][0];
    for(int f = 0; f < 16; f++)
        for (int m = 0; m < 4; m++) 
        {
            fixp_bn_layer_1_weights[f][m] = (wt_t) bn_layer_1_weights[f][m];
            if (bn_layer_1_weights[f][m] > bn1_max)
                bn1_max = bn_layer_1_weights[f][m];
            if (bn_layer_1_weights[f][m] < bn1_min)
                bn1_min = bn_layer_1_weights[f][m];
        }
    for(int f = 0; f < 32; f++)
        for (int m = 0; m < 4; m++)
        {
            fixp_bn_layer_2_weights[f][m] = (wt_t) bn_layer_2_weights[f][m];
            if (bn_layer_2_weights[f][m] > bn1_max)
                bn1_max = bn_layer_2_weights[f][m];
            if (bn_layer_2_weights[f][m] < bn1_min)
                bn1_min = bn_layer_2_weights[f][m];
        }

    for(int f = 0; f < 64; f++)
        for (int m = 0; m < 4; m++) 
        {
            fixp_bn_layer_3_weights[f][m] = (wt_t) bn_layer_3_weights[f][m];
            if (bn_layer_3_weights[f][m] > bn1_max)
                bn1_max = bn_layer_3_weights[f][m];
            if (bn_layer_3_weights[f][m] < bn1_min)
                bn1_min = bn_layer_3_weights[f][m];
        }

    for(int f = 0; f < 128; f++)
        for (int m = 0; m < 4; m++)
        {
            fixp_bn_layer_4_weights[f][m] = (wt_t) bn_layer_4_weights[f][m];
            if (bn_layer_4_weights[f][m] > bn1_max)
                bn1_max = bn_layer_4_weights[f][m];
            if (bn_layer_4_weights[f][m] < bn1_min)
                bn1_min = bn_layer_4_weights[f][m];
        }

    for(int f = 0; f < 256; f++)
        for (int m = 0; m < 4; m++) 
        {
            fixp_bn_layer_5_weights[f][m] = (wt_t) bn_layer_5_weights[f][m];
            if (bn_layer_5_weights[f][m] > bn1_max)
                bn1_max = bn_layer_5_weights[f][m];
            if (bn_layer_5_weights[f][m] < bn1_min)
                bn1_min = bn_layer_5_weights[f][m];
        }

    for(int f = 0; f < 512; f++)
        for (int m = 0; m < 4; m++) 
        {
            fixp_bn_layer_6_weights[f][m] = (wt_t) bn_layer_6_weights[f][m];
            if (bn_layer_6_weights[f][m] > bn1_max)
                bn1_max = bn_layer_6_weights[f][m];
            if (bn_layer_6_weights[f][m] < bn1_min)
                bn1_min = bn_layer_6_weights[f][m];
        }

    for(int f = 0; f < 1024; f++)
        for (int m = 0; m < 4; m++) 
        {
            fixp_bn_layer_7_weights[f][m] = (wt_t) bn_layer_7_weights[f][m];
            if (bn_layer_7_weights[f][m] > bn1_max)
                bn1_max = bn_layer_7_weights[f][m];
            if (bn_layer_7_weights[f][m] < bn1_min)
                bn1_min = bn_layer_7_weights[f][m];
        }

    for(int f = 0; f < 256; f++)
        for (int m = 0; m < 4; m++) 
        {
            fixp_bn_layer_8_weights[f][m] = (wt_t) bn_layer_8_weights[f][m];
            if (bn_layer_8_weights[f][m] > bn1_max)
                bn1_max = bn_layer_8_weights[f][m];
            if (bn_layer_8_weights[f][m] < bn1_min)
                bn1_min = bn_layer_8_weights[f][m];
        }
    for(int f = 0; f < 512; f++)
        for (int m = 0; m < 4; m++) 
        {
            fixp_bn_layer_9_weights[f][m] = (wt_t) bn_layer_9_weights[f][m];
            if (bn_layer_9_weights[f][m] > bn1_max)
                bn1_max = bn_layer_9_weights[f][m];
            if (bn_layer_9_weights[f][m] < bn1_min)
                bn1_min = bn_layer_9_weights[f][m];
        }

    for(int f = 0; f < 128; f++)
        for (int m = 0; m < 4; m++) 
        {
            fixp_bn_layer_11_weights[f][m] = (wt_t) bn_layer_11_weights[f][m];
            if (bn_layer_11_weights[f][m] > bn1_max)
                bn1_max = bn_layer_11_weights[f][m];
            if (bn_layer_11_weights[f][m] < bn1_min)
                bn1_min = bn_layer_11_weights[f][m];
        }

    for(int f = 0; f < 256; f++)
        for (int m = 0; m < 4; m++) 
        {
            fixp_bn_layer_12_weights[f][m] = (wt_t) bn_layer_12_weights[f][m];
            if (bn_layer_12_weights[f][m] > bn1_max)
                bn1_max = bn_layer_12_weights[f][m];
            if (bn_layer_12_weights[f][m] < bn1_min)
                bn1_min = bn_layer_12_weights[f][m];
        }

    std::cout << "bn_max: " << bn1_max << std::endl;
    std::cout << "bn_min: " << bn1_min << std::endl;

    //Bias
    for(int f = 0; f < 255; f++)
        fixp_bias_layer_10[f] = (wt_t) bias_layer_10[f];

    for(int f = 0; f < 255; f++)
        fixp_bias_layer_13[f] = (wt_t) bias_layer_13[f];
}

//--------------------------------------------------------------------------
// This is where fun begins.
//--------------------------------------------------------------------------
int main ()
{
    long double mse = 0.0;
    
    // Read reference inputs and output
    read_bin_files();
   
    // Convert to fixed-point types 
    convert_type();

    #ifdef CMODEL_SIM
        std::cout << "Beginning C model simulation..." << std::endl;
        //Layer 0 - Convolution layer 1
        model_conv_bn ((fm_t *)fixp_input_image,
                    (wt_t *)fixp_conv_layer_1_weights,
                    (wt_t *)fixp_bn_layer_1_weights,
                    (fm_t *)fixp_conv_layer_output_feature_map,
                    3,
                    416,
                    416,
                    16,
                    3,
                    3
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(16*416*416), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 1 - Maxpooling layer
        model_maxpool2D ((fm_t *)fixp_conv_layer_input_feature_map,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        16,
                        416,
                        416,
                        2
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(16*208*208), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 2 - Convolution layer 2
        model_conv_bn ((fm_t *)fixp_conv_layer_input_feature_map,
                    (wt_t *)fixp_conv_layer_2_weights,
                    (wt_t *)fixp_bn_layer_2_weights,
                    (fm_t *)fixp_conv_layer_output_feature_map,
                    16,
                    208,
                    208,
                    32,
                    3,
                    3
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(32*208*208), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 3 - Maxpooling layer
        model_maxpool2D((fm_t *)fixp_conv_layer_input_feature_map,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        32,
                        208,
                        208,
                        2
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(32*104*104), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 4 - Convolution layer 3
        model_conv_bn ((fm_t *)fixp_conv_layer_input_feature_map,
                    (wt_t *)fixp_conv_layer_3_weights,
                    (wt_t *)fixp_bn_layer_3_weights,
                    (fm_t *)fixp_conv_layer_output_feature_map,
                    32,
                    104,
                    104,
                    64,
                    3,
                    3
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(64*104*104), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 5 - Maxpooling layer
        model_maxpool2D((fm_t *)fixp_conv_layer_input_feature_map,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        64,
                        104,
                        104,
                        2
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(64*52*52), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 6 - Convolution layer 4
        model_conv_bn ((fm_t *)fixp_conv_layer_input_feature_map,
                    (wt_t *)fixp_conv_layer_4_weights,
                    (wt_t *)fixp_bn_layer_4_weights,
                    (fm_t *)fixp_conv_layer_output_feature_map,
                    64,
                    52,
                    52,
                    128,
                    3,
                    3
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(128*52*52), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 7 - Maxpooling layer
        model_maxpool2D((fm_t *)fixp_conv_layer_input_feature_map,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        128,
                        52,
                        52,
                        2
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(128*26*26), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 8 - Convolution layer 5
        model_conv_bn ((fm_t *)fixp_conv_layer_input_feature_map,
                    (wt_t *)fixp_conv_layer_5_weights,
                    (wt_t *)fixp_bn_layer_5_weights,
                    (fm_t *)fixp_conv_layer_output_feature_map,
                    128,
                    26,
                    26,
                    256,
                    3,
                    3
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(256*26*26), (fm_t *)fixp_conv_layer_input_feature_map);
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(256*26*26), (fm_t *)fixp_conv_layer_8_output_feature_map);
        //Layer 9 - Maxpooling layer
        model_maxpool2D((fm_t *)fixp_conv_layer_input_feature_map,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        256,
                        26,
                        26,
                        2
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(256*13*13), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 10 - Convolution layer 6
        model_conv_bn ((fm_t *)fixp_conv_layer_input_feature_map,
                    (wt_t *)fixp_conv_layer_6_weights,
                    (wt_t *)fixp_bn_layer_6_weights,
                    (fm_t *)fixp_conv_layer_output_feature_map,
                    256,
                    13,
                    13,
                    512,
                    3,
                    3
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(512*13*13), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 11
        model_maxpool2D((fm_t *)fixp_conv_layer_input_feature_map,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        512,
                        13,
                        13,
                        1
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(256*13*13), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 12 - Convolution layer 7
        model_conv_bn ((fm_t *)fixp_conv_layer_input_feature_map,
                        (wt_t *)fixp_conv_layer_7_weights,
                        (wt_t *)fixp_bn_layer_7_weights,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        512,
                        13,
                        13,
                        1024,
                        3,
                        3
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(1024*13*13), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 13 - Convolution layer 8
        model_conv_bn ((fm_t *)fixp_conv_layer_input_feature_map,
                        (wt_t *)fixp_conv_layer_8_weights,
                        (wt_t *)fixp_bn_layer_8_weights,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        1024,
                        13,
                        13,
                        256,
                        1,
                        1
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(256*13*13), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 17 - Route 13
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(256*13*13), (fm_t *)fixp_conv_layer_13_output_feature_map);
        //Layer 14 - Convolution layer 9
        model_conv_bn ((fm_t *)fixp_conv_layer_input_feature_map,
                        (wt_t *)fixp_conv_layer_9_weights,
                        (wt_t *)fixp_bn_layer_9_weights,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        256,
                        13,
                        13,
                        512,
                        3,
                        3
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(512*13*13), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 15 - Convolution layer 10
        model_conv ((fm_t *)fixp_conv_layer_input_feature_map,
                    (wt_t *)fixp_conv_layer_10_weights,
                    (wt_t *)fixp_bias_layer_10,
                    (fm_t *)fixp_conv_layer_output_feature_map,
                    512,
                    13,
                    13,
                    255,
                    1,
                    1
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(255*13*13), (fm_t *)fixp_conv_layer_input_feature_map);
        //TODO: Layer 16 - Insert call to Yolo layer
        model_yolo ((fm_t *)fixp_conv_layer_input_feature_map,
                    (fm_t *)fixp_conv_layer_output_feature_map,
                    255,
                    13,
                    13,
                    fixp_yolo_layer_16_stride, // TODO: check whether this layer should have 16 or 32
                    (wt_t *) fixp_yolo_layer_16_anchor // //TODO: update the anchor

        );

        // print_layers((fm_t *)fixp_conv_layer_input_feature_map,
        //             (fm_t *)fixp_conv_layer_output_feature_map,
        //             255,
        //             13,
        //             13
        // );
    
        //Layer 18 - Convolution layer 11
        model_conv_bn  ((fm_t *)fixp_conv_layer_13_output_feature_map,
                        (wt_t *)fixp_conv_layer_11_weights,
                        (wt_t *)fixp_bn_layer_11_weights,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        256,
                        13,
                        13,
                        128,
                        1,
                        1
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(128*13*13), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 19 - Upsampling
        model_upsample ((fm_t *)fixp_conv_layer_input_feature_map,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        128,
                        13,
                        13
        );
        //Layer 20 - Route 19,8
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(128*26*26), (fm_t *)fixp_conv_layer_input_feature_map);
        std::copy((fm_t *)fixp_conv_layer_8_output_feature_map, (fm_t *)fixp_conv_layer_8_output_feature_map+(256*26*26), ((fm_t *)fixp_conv_layer_input_feature_map)+(128*26*26));
        //Layer 21 - Convolution layer 12
        model_conv_bn  ((fm_t *)fixp_conv_layer_input_feature_map,
                        (wt_t *)fixp_conv_layer_12_weights,
                        (wt_t *)fixp_bn_layer_12_weights,
                        (fm_t *)fixp_conv_layer_output_feature_map,
                        384,
                        26,
                        26,
                        256,
                        3,
                        3
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(256*26*26), (fm_t *)fixp_conv_layer_input_feature_map);
        //Layer 22 - Convolution layer 13
        model_conv ((fm_t *)fixp_conv_layer_input_feature_map,
                    (wt_t *)fixp_conv_layer_13_weights,
                    (wt_t *)fixp_bias_layer_13,
                    (fm_t *)fixp_conv_layer_output_feature_map,
                    256,
                    26,
                    26,
                    255,
                    1,
                    1
        );
        std::copy((fm_t *)fixp_conv_layer_output_feature_map, (fm_t *)fixp_conv_layer_output_feature_map+(255*26*26), (fm_t *)fixp_conv_layer_input_feature_map);
        //TODO: Layer 23 - Insert call to Yolo layer
        model_yolo ((fm_t *)fixp_conv_layer_input_feature_map,
                    (fm_t *)fixp_conv_layer_output_feature_map,
                    255,
                    26,
                    26,
                    fixp_yolo_layer_23_stride, // TODO: check whether this layer should have 16 or 32
                    (wt_t *) fixp_yolo_layer_23_anchor // //TODO: update the anchor

        );

        // print_layers((fm_t *)fixp_conv_layer_input_feature_map,
        //             (fm_t *)fixp_conv_layer_output_feature_map,
        //             255,
        //             26,
        //             26
        // );

        std::cout << "C model simulation complete!\n" << std::endl;
    #else
        std::cout << "Beginning HLS tiled-convolution simulation..." << std::endl;
        yolov3_tiny(fixp_input_image,
                    fixp_conv_layer_1_weights,
                    fixp_conv_layer_2_weights,
                    fixp_conv_layer_3_weights,
                    fixp_conv_layer_4_weights,
                    fixp_conv_layer_5_weights,
                    fixp_conv_layer_6_weights,
                    fixp_conv_layer_7_weights,
                    fixp_conv_layer_8_weights,
                    fixp_conv_layer_9_weights,
                    fixp_conv_layer_10_weights,
                    fixp_conv_layer_11_weights,
                    fixp_conv_layer_12_weights,
                    fixp_conv_layer_13_weights,
                    fixp_bias_layer_10,
                    fixp_bias_layer_13,
                    fixp_conv_layer_input_feature_map,
                    fixp_conv_layer_output_feature_map,
                    fixp_conv_layer_13_output_feature_map,
                    fixp_conv_layer_8_output_feature_map
        );
        std::cout << "Tiled-convolution simulation complete!\n" << std::endl;
    #endif
    
    //Compute MSE
    for(int f = 0; f < 16; f++)
    {
    //     for(int i = 0; i < 184; i++)
    //     {
    //         for(int j = 0; j < 320; j++)
    //         {
    //             mse += std::pow((conv_layer_golden_output_feature_map[f][i][j] 
    //                           - (float) fixp_conv_layer_output_feature_map[f][i][j]), 2);
    //         }
    //     }
        
            // Prints sample output values for comparison
            // Uncomment for debugging
            //cout << "Golden Output: " << conv_layer_golden_output_feature_map[f][0][0] << std::endl;
            //cout << "Actual Output: " << fixp_conv_layer_output_feature_map[f][0][0] << std::endl;
            //cout << std::endl;
     }
    
    // mse = mse / (64 * 184 * 320);

    // std::cout << "Output MSE:  " << mse << std::endl;
    
    // std::cout << "----------------------------------------" << std::endl;
    #ifdef CSIM_DEBUG
        // if(mse > 0 && mse < std::exp(-13))
        // {
        //     std::cout << "Simulation SUCCESSFUL!!!" << std::endl;
        // }
        // else
        // {
        //     std::cout << "Simulation FAILED :(" << std::endl;
        // }
    #else
        // if(mse > 0 && mse < std::exp(-3))
        // {
        //     std::cout << "Simulation SUCCESSFUL!!!" << std::endl;
        // }
        // else
        // {
        //     std::cout << "Simulation FAILED :(" << std::endl;
        // }
    #endif
    std::cout << "----------------------------------------" << std::endl;

    return 0;
}
