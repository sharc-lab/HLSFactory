//--------------------------------------------------------------------------
// Test bench for your convolution codes.
//
// You should not need to modify this, except for debugging.
//
// Remove any print statements in your submission codes!
//--------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <cmath>

#include "gradient.h"
#include "io.h"
#include "cmodel/fp.h"
#include "cmodel/bp_single_class.h"

using namespace std;

//--------------------------------------------------------------------------
// Set up the global variables for all the layers
//--------------------------------------------------------------------------

// float versions for csim

float layer1_ifmap[3][IM_SIZE][IM_SIZE];        // input image

fm_t fixp_layer1_ifmap[3][IM_SIZE][IM_SIZE];        // input image
fm_t fixp_layer2_ifmap[32][IM_SIZE][IM_SIZE];       // output of conv1
fm_t fixp_layer3_ifmap[32][IM_SIZE][IM_SIZE];       // output of conv2
fm_t fixp_layer4_ifmap[32][IM_SIZE/2][IM_SIZE/2];   // output of max_pool1
fm_t fixp_layer5_ifmap[64][IM_SIZE/2][IM_SIZE/2];   // output of conv3
fm_t fixp_layer6_ifmap[64][IM_SIZE/2][IM_SIZE/2];   // output of conv4
fm_t fixp_layer7_ifmap[64][IM_SIZE/4][IM_SIZE/4];   // output of max_pool2
fm_t fixp_layer8_ifmap[128];                        // output of fc1
fm_t fixp_layer9_ifmap[128];                        // output of ReLU1
fm_t fixp_layer10_ifmap[10];                        // output of fc2

// fixed point versions of above variables

float conv1_weights[32][3][3][3];
float conv1_bias[32];
float conv2_weights[32][32][3][3];
float conv2_bias[32];
float conv3_weights[64][32][3][3];
float conv3_bias[64];
float conv4_weights[64][64][3][3];
float conv4_bias[64];
float fc1_weights[128][4096];
float fc1_bias[128];
float fc2_weights[10][128];
float fc2_bias[10];

wt_t fixp_conv1_weights[32][3][3][3];
wt_t fixp_conv1_bias[32];
wt_t fixp_conv2_weights[32][32][3][3];
wt_t fixp_conv2_bias[32];
wt_t fixp_conv3_weights[64][32][3][3];
wt_t fixp_conv3_bias[64];
wt_t fixp_conv4_weights[64][64][3][3];
wt_t fixp_conv4_bias[64];
wt_t fixp_fc1_weights[128][4096];
wt_t fixp_fc1_bias[128];
wt_t fixp_fc2_weights[10][128];
wt_t fixp_fc2_bias[10];

fm_t fixp_grad1[128];
fm_t fixp_grad2[128];
fm_t fixp_grad3[4096];
fm_t fixp_grad4[64][IM_SIZE/4][IM_SIZE/4];
fm_t fixp_grad5[64][IM_SIZE/2][IM_SIZE/2];
fm_t fixp_grad6[64][IM_SIZE/2][IM_SIZE/2];
fm_t fixp_grad7[32][IM_SIZE/2][IM_SIZE/2];
fm_t fixp_grad8[32][IM_SIZE][IM_SIZE];
fm_t fixp_grad9[32][IM_SIZE][IM_SIZE];
fm_t fixp_grad10[3][IM_SIZE][IM_SIZE];

mk_t fixp_mask_maxpool2[64][IM_SIZE/4][IM_SIZE/4];
mk_t fixp_mask_maxpool1[32][IM_SIZE/2][IM_SIZE/2];

//--------------------------------------------------------------------------
// Read the reference files into test bench arrays
//--------------------------------------------------------------------------

void read_bin_files()
{
    read_input_feature <3,IM_SIZE,IM_SIZE> (layer1_ifmap);

    read_conv_weight <32,3,3,3> ("bin/conv_layer1_weights.bin", conv1_weights);
    read_conv_bias <32> ("bin/conv_layer1_bias.bin", conv1_bias);

    read_conv_weight <32,32,3,3> ("bin/conv_layer2_weights.bin", conv2_weights);
    read_conv_bias <32> ("bin/conv_layer2_bias.bin", conv2_bias);

    read_conv_weight <64,32,3,3> ("bin/conv_layer3_weights.bin", conv3_weights);
    read_conv_bias <64> ("bin/conv_layer3_bias.bin", conv3_bias);

    read_conv_weight <64,64,3,3> ("bin/conv_layer4_weights.bin", conv4_weights);
    read_conv_bias <64> ("bin/conv_layer4_bias.bin", conv4_bias);

    read_fc_weight <128,4096> ("bin/fc1_weights.bin", fc1_weights);
    read_fc_bias <128> ("bin/fc1_bias.bin", fc1_bias);

    read_fc_weight <10,128> ("bin/fc2_weights.bin", fc2_weights);
    read_fc_bias <10> ("bin/fc2_bias.bin", fc2_bias);
}

//--------------------------------------------------------------------------
// Convert the data types of every array element for specified 
// configuration.
//--------------------------------------------------------------------------

void convert_type()
{
    convert_input_3d <3,IM_SIZE,IM_SIZE> (layer1_ifmap, fixp_layer1_ifmap);

    convert_conv_layer_params <3,32> (
        conv1_weights,
        conv1_bias,
        fixp_conv1_weights,
        fixp_conv1_bias
    );

    convert_conv_layer_params <32,32> (
        conv2_weights,
        conv2_bias,
        fixp_conv2_weights,
        fixp_conv2_bias
    );

    convert_conv_layer_params <32,64> (
        conv3_weights,
        conv3_bias,
        fixp_conv3_weights,
        fixp_conv3_bias
    );

    convert_conv_layer_params <64,64> (
        conv4_weights,
        conv4_bias,
        fixp_conv4_weights,
        fixp_conv4_bias
    );

    convert_fc_layer_params <4096, 128> (
        fc1_weights,
        fc1_bias,
        fixp_fc1_weights,
        fixp_fc1_bias
    );

    convert_fc_layer_params <128, 10> (
        fc2_weights,
        fc2_bias,
        fixp_fc2_weights,
        fixp_fc2_bias
    );

}   

//--------------------------------------------------------------------------
// CMODEL for simulation 
//--------------------------------------------------------------------------

void cmodel_conv_fp(){
    model_conv <32,3,IM_SIZE,IM_SIZE> (
        fixp_layer1_ifmap,
        fixp_conv1_weights,
        fixp_conv1_bias,
        fixp_layer2_ifmap
    );

    model_conv <32,32,IM_SIZE,IM_SIZE>(
        fixp_layer2_ifmap,
        fixp_conv2_weights,
        fixp_conv2_bias,
        fixp_layer3_ifmap
    );

    model_max_pool <32,IM_SIZE,IM_SIZE>(
        fixp_layer3_ifmap,
        fixp_layer4_ifmap,
        fixp_mask_maxpool1
    );

    model_conv <64,32,IM_SIZE/2,IM_SIZE/2>(
        fixp_layer4_ifmap,
        fixp_conv3_weights,
        fixp_conv3_bias,
        fixp_layer5_ifmap
    );

    model_conv <64,64,IM_SIZE/2,IM_SIZE/2>(
        fixp_layer5_ifmap,
        fixp_conv4_weights,
        fixp_conv4_bias,
        fixp_layer6_ifmap
    );

    model_max_pool <64,IM_SIZE/2,IM_SIZE/2>(
        fixp_layer6_ifmap,
        fixp_layer7_ifmap,
        fixp_mask_maxpool2
    );

    model_conv_flatten_fc <64,8,8,128, 64*8*8> (
        fixp_layer7_ifmap,
        fixp_fc1_weights,
        fixp_fc1_bias,
        fixp_layer8_ifmap
    );

    model_relu_fc <128> (
        fixp_layer8_ifmap,
        fixp_layer9_ifmap
    );

    model_mat_mul <128,10> (
        fixp_layer9_ifmap,
        fixp_fc2_weights,
        fixp_fc2_bias,
        fixp_layer10_ifmap
    );

}

void disp_probs()
{  
    cout << "Displaying class probabilities" << endl;
    int num_classes = 10;
    for(int i=0; i<num_classes; i++){
        cout << fixp_layer10_ifmap[i] << endl;
    }
}

void disp_grads()
{
    cout << "Displaying gradient values" << endl;
    for(int i=0; i<IM_SIZE; i++){
        cout << fixp_grad10[0][0][i] << endl;
    }
}

void cmodel_conv_bp(int class_index)
{

    for(int i=0; i<128; i++){
        fixp_grad1[i] = (fm_t) fixp_fc2_weights[class_index][i];
    }

    model_relu_fc_grad_single <128> (
        fixp_grad1,
        fixp_layer8_ifmap,
        fixp_grad2
    );

    model_fc_grad_single <128, 4096> (
        fixp_grad2,
        fixp_fc1_weights,
        fixp_grad3
    );

    model_reshape_single <64,8,8> (
        fixp_grad3,
        fixp_grad4
    );

    model_upsample_single <64, IM_SIZE/4, IM_SIZE/4> (
        fixp_grad4,
        fixp_mask_maxpool2,
        fixp_grad5
    );

    model_flipped_conv_single <64,64,IM_SIZE/2,IM_SIZE/2> (
        fixp_grad5,
        fixp_conv4_weights,
        fixp_grad6
    );

    model_flipped_conv_single <32,64,IM_SIZE/2,IM_SIZE/2> (
        fixp_grad6,
        fixp_conv3_weights,
        fixp_grad7
    );

    model_upsample_single <32, IM_SIZE/2, IM_SIZE/2> (
        fixp_grad7,
        fixp_mask_maxpool1,
        fixp_grad8
    );

    model_flipped_conv_single <32,32,IM_SIZE,IM_SIZE> (
        fixp_grad8,
        fixp_conv2_weights,
        fixp_grad9
    );

    model_flipped_conv_single <3,32,IM_SIZE,IM_SIZE> (
        fixp_grad9,
        fixp_conv1_weights,
        fixp_grad10
    );
}

//--------------------------------------------------------------------------
// HLS Model for simulation followed by pragma optimization
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
// This is where fun begins.
//--------------------------------------------------------------------------
int main ()
{
    read_bin_files();
    
    convert_type();

    #ifdef CMODEL_SIM
    cout << "Beginning C model simulation..." << std::endl;
        cmodel_conv_fp();

        for (int i=0; i<32; i++){
            cout << fixp_layer2_ifmap[0][0][i] << endl;
        }
        // disp_probs();
        cmodel_conv_bp(0);

        for(int i=0; i<16; i++){
            for(int j=0; j<16; j++){
                std::cout << fixp_grad5[0][i][j] << std::endl;
            }
        }
        // disp_grads();
    cout << "C model simulation complete!\n" << std::endl;
    #else
    cout << "Beginning HLS tiled-convolution simulation..." << std::endl;
        tiled_conv(
            fixp_layer1_ifmap,
            fixp_layer2_ifmap,
            fixp_layer3_ifmap,
            fixp_layer4_ifmap,
            fixp_layer5_ifmap,
            fixp_layer6_ifmap,
            fixp_layer7_ifmap,
            fixp_layer8_ifmap,
            fixp_layer9_ifmap,
            fixp_layer10_ifmap,
            fixp_conv1_weights,
            fixp_conv1_bias,
            fixp_conv2_weights,
            fixp_conv2_bias,
            fixp_conv3_weights,
            fixp_conv3_bias,
            fixp_conv4_weights,
            fixp_conv4_bias,
            fixp_fc1_weights,
            fixp_fc1_bias,
            fixp_fc2_weights,
            fixp_fc2_bias,
            fixp_grad1,
            fixp_grad2,
            fixp_grad3,
            fixp_grad4,
            fixp_grad5,
            fixp_grad6,
            fixp_grad7,
            fixp_grad8,
            fixp_grad9,
            fixp_grad10,
            fixp_mask_maxpool2,
            fixp_mask_maxpool1
        );
    cout << "Tiled-convolution simulation complete!\n" << std::endl;
    #endif

    return 0;
}
