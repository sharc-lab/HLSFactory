#include <iostream>
#include <fstream>
#include <cmath>
#include "conv.h" 

using namespace std;

//renamed input_feature_maps to output_feature_maps because the bin is actually the output of the layer
float output_feature_map_map1_golden[1][32][187];
float conv_layer_weights1[32][1][5];
float conv_layer_bias1[32];

float conv_layer_weights2[32][32][5];
float conv_layer_bias2[32];

float conv_layer_weights3[32][32][5];
float conv_layer_bias3[32];

float conv_layer_weights4[32][32][5];
float conv_layer_bias4[32];

float dense1_layer_weights[32][64];
float dense1_layer_bias[32];

float dense2_layer_weights[5][32];
float dense2_layer_bias[5];

fm_t fixp_conv_layer_input_feature_map1[1][1][187];
fm_t output_feature_map[5];

wt_t fixp_conv_layer_weights1[32][1][5];
wt_t fixp_conv_layer_bias1[32];
wt_t fixp_conv_layer_weights2[32][32][5];
wt_t fixp_conv_layer_bias2[32];
wt_t fixp_conv_layer_weights3[32][32][5];
wt_t fixp_conv_layer_bias3[32];
wt_t fixp_conv_layer_weights4[32][32][5];
wt_t fixp_conv_layer_bias4[32];

// declare weights and biases for dense1 and dense2
wt_t fixp_dense1_weights[32][64]; 
wt_t fixp_dense1_bias[32];
wt_t fixp_dense2_weights[5][32];
wt_t fixp_dense2_bias[5];

//Variables used to verify functionality
float output_conv1[1][32][187];
float output_conv2[1][32][92];
float output_max1[1][32][92];
float output_max2[1][32][44];
float output_max3[1][32][20];
float output_conv3[1][32][44];
float output_conv4[1][32][20];
float output_max4[1][32][8];
float output_max5[1][32][2];
float golden_output[5];


void read_bin_files()
{    
    /* Read bin files to check correct functionality */

    // conv1 output
    ifstream ifs_conv1("../bin/conv1.bin", ios::in | ios::binary);
    ifs_conv1.read((char*)(**output_conv1), 1*32*187*sizeof(float));
    ifs_conv1.close();

    // conv2 output
    ifstream ifs_conv2("../bin/conv2.bin", ios::in | ios::binary);
    ifs_conv2.read((char*)(**output_conv2), 1*32*92*sizeof(float));
    ifs_conv2.close();

    // max1 output
    ifstream ifs_max1("../bin/max1.bin", ios::in | ios::binary);
    ifs_max1.read((char*)(**output_max1), 1*32*92*sizeof(float));
    ifs_max1.close();

    // max2 output
    ifstream ifs_max2("../bin/max2.bin", ios::in | ios::binary);
    ifs_max2.read((char*)(**output_max2), 1*32*44*sizeof(float));
    ifs_max2.close();

    //conv3 output
    ifstream ifs_conv3("../bin/conv3.bin", ios::in | ios::binary);
    ifs_conv3.read((char*)(**output_conv3), 1*32*44*sizeof(float));
    ifs_conv3.close();

    //conv4 output
    ifstream ifs_conv4("../bin/conv4.bin", ios::in | ios::binary);
    ifs_conv4.read((char*)(**output_conv4), 1*32*20*sizeof(float));
    ifs_conv4.close();


        // max3 output
    ifstream ifs_max3("../bin/max3.bin", ios::in | ios::binary);
    ifs_max3.read((char*)(**output_max3), 1*32*20*sizeof(float));
    ifs_max3.close();

        // max4 output
    ifstream ifs_max4("../bin/max4.bin", ios::in | ios::binary);
    ifs_max4.read((char*)(**output_max4), 1*32*8*sizeof(float));
    ifs_max4.close();

           // max5 output
    ifstream ifs_max5("../bin/max5.bin", ios::in | ios::binary);
    ifs_max5.read((char*)(**output_max5), 1*32*2*sizeof(float));
    ifs_max5.close();

      //Final golden output
    ifstream ifs_dense2("../bin/dense2.bin", ios::in | ios::binary);
    ifs_dense2.read((char*)(golden_output), 1*5*sizeof(float));
    ifs_dense2.close();
    

    /* Read weights and biases*/


    // Weights 1
    ifstream ifs_conv1_weights("../bin/conv1_weights.bin", ios::in | ios::binary);
    ifs_conv1_weights.read((char*)(**conv_layer_weights1), 32*1*5*sizeof(float));
    ifs_conv1_weights.close();
    
    // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        for(int c = 0; c < 1; c++)
            for(int m = 0; m < 5; m++)
                    fixp_conv_layer_weights1[f][c][m] = (wt_t) conv_layer_weights1[f][c][m];
    
    // Bias 1
    ifstream ifs_conv1_bias("../bin/conv1_bias.bin", ios::in | ios::binary);
    ifs_conv1_bias.read((char*)(conv_layer_bias1), 32*sizeof(float));
    ifs_conv1_bias.close();
    
    // // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        fixp_conv_layer_bias1[f] = (wt_t) conv_layer_bias1[f];

    // Weights 2
    ifstream ifs_conv2_weights("../bin/conv2_weights.bin", ios::in | ios::binary);
    ifs_conv2_weights.read((char*)(**conv_layer_weights2), 32*32*5*sizeof(float));
    ifs_conv2_weights.close();
    
    // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        for(int c = 0; c < 32; c++)
            for(int m = 0; m < 5; m++)
                    fixp_conv_layer_weights2[f][c][m] = (wt_t) conv_layer_weights2[f][c][m];
    
    // Bias 2
    ifstream ifs_conv2_bias("../bin/conv2_bias.bin", ios::in | ios::binary);
    ifs_conv2_bias.read((char*)(conv_layer_bias2), 32*sizeof(float));
    ifs_conv2_bias.close();
    
    // // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        fixp_conv_layer_bias2[f] = (wt_t) conv_layer_bias2[f];

    // Weights 3
    ifstream ifs_conv3_weights("../bin/conv3_weights.bin", ios::in | ios::binary);
    ifs_conv3_weights.read((char*)(**conv_layer_weights3), 32*32*5*sizeof(float));
    ifs_conv3_weights.close();
    
    // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        for(int c = 0; c < 32; c++)
            for(int m = 0; m < 5; m++)
                    fixp_conv_layer_weights3[f][c][m] = (wt_t) conv_layer_weights3[f][c][m];


    // Bias 3
    ifstream ifs_conv3_bias("../bin/conv3_bias.bin", ios::in | ios::binary);
    ifs_conv3_bias.read((char*)(conv_layer_bias3), 32*sizeof(float));
    ifs_conv3_bias.close();
    
    // // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        fixp_conv_layer_bias3[f] = (wt_t) conv_layer_bias3[f];


    // Weights 4
    ifstream ifs_conv4_weights("../bin/conv4_weights.bin", ios::in | ios::binary);
    ifs_conv4_weights.read((char*)(**conv_layer_weights4), 32*32*5*sizeof(float));
    ifs_conv4_weights.close();
    
    // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        for(int c = 0; c < 32; c++)
            for(int m = 0; m < 5; m++)
                    fixp_conv_layer_weights4[f][c][m] = (wt_t) conv_layer_weights4[f][c][m];
    
    // Bias 4
    ifstream ifs_conv4_bias("../bin/conv4_bias.bin", ios::in | ios::binary);
    ifs_conv4_bias.read((char*)(conv_layer_bias4), 32*sizeof(float));
    ifs_conv4_bias.close();
    
    // // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        fixp_conv_layer_bias4[f] = (wt_t) conv_layer_bias4[f];


    ifstream ifs_dense1_bias("../bin/dense1_bias.bin", ios::in | ios::binary);
    ifs_dense1_bias.read((char*)(dense1_layer_bias), 32*sizeof(float));
    ifs_dense1_bias.close();

     // // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        fixp_dense1_bias[f] = (wt_t) dense1_layer_bias[f];


    ifstream ifs_dense2_bias("../bin/dense2_bias.bin", ios::in | ios::binary);
    ifs_dense2_bias.read((char*)(dense2_layer_bias), 5*sizeof(float));
    ifs_dense2_bias.close();

     // // Typecast to fixed-point 
    for(int f = 0; f < 5; f++)
        fixp_dense2_bias[f] = (wt_t) dense2_layer_bias[f];


    ifstream ifs_dense1_weights("../bin/dense1_weights.bin", ios::in | ios::binary);
    ifs_dense1_weights.read((char*)(dense1_layer_weights), 32*64*sizeof(float));
    ifs_dense1_weights.close();

      // Typecast to fixed-point 
    for(int f = 0; f < 32; f++)
        for(int c = 0; c < 64; c++)
                    fixp_dense1_weights[f][c] = (wt_t) dense1_layer_weights[f][c];


    ifstream ifs_dense2_weights("../bin/dense2_weights.bin", ios::in | ios::binary);
    ifs_dense2_weights.read((char*)(dense2_layer_weights), 5*32*sizeof(float));
    ifs_dense2_weights.close();

          // Typecast to fixed-point 
    for(int f = 0; f < 5; f++)
        for(int c = 0; c < 32; c++)
                    fixp_dense2_weights[f][c] = (wt_t) dense2_layer_weights[f][c];

    
}

int main(){
    read_bin_files();

    // Initialize input with ones
    for (int i = 0; i < 1; i++)
    {
        for (int j = 0; j < 1; j++)
        {
            for (int k = 0; k < 187; k++)
            {
                fixp_conv_layer_input_feature_map1[i][j][k] = 1;
            }
        }
    }

    std::cout << "Beginning C model..." << std::endl;

    tiled_conv(
        fixp_conv_layer_input_feature_map1,
        fixp_conv_layer_weights1,
        fixp_conv_layer_bias1,
        fixp_conv_layer_weights2,
        fixp_conv_layer_bias2,
        fixp_conv_layer_weights3,
        fixp_conv_layer_bias3,
        fixp_conv_layer_weights4,
        fixp_conv_layer_bias4,
        fixp_dense1_weights,
        fixp_dense1_bias,
        fixp_dense2_weights,
        fixp_dense2_bias,
        output_feature_map);
    

    cout << "Output feature map" << endl;
    for(int f = 0; f < 5; f++){
        cout << output_feature_map[f] << "   ";
    }
    cout << endl;

    cout << "Expected feature map" << endl;
    for(int f = 0; f < 5; f++){
        cout << golden_output[f] << "   ";
    }
    cout << endl;

  
}
