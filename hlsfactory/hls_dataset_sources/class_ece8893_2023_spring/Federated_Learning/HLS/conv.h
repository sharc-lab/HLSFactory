///////////////////////////////////////////////////////////////////////////////
// Course:      ECE8893 - Parallel Programming for FPGAs
// Filename:    conv.h
// Description: Header file for tiling-based synthesizable implementation of 
//              training of a neural network on MNIST dataset.
///////////////////////////////////////////////////////////////////////////////

#ifndef CONV_H_
#define CONV_H_

#include <iostream>
#include <ap_fixed.h>

//--------------------------------------------------------------------------
// Type conversions for simulation and synthesis
//--------------------------------------------------------------------------
#ifdef  CSIM_DEBUG
    typedef float fm_t;
    typedef float fm_tsum;
    typedef float wt_t;
#else
    typedef ap_fixed<16,3> fm_t;
    typedef ap_fixed<16,8> fm_tsum;
    typedef ap_fixed<16,3> wt_t;
#endif

//--------------------------------------------------------------------------
// Configuration of ResNet-50's first 7 x 7 convolution layer with HD input 
//--------------------------------------------------------------------------
#define SET_SIZE          2
#define IN_FM_DEPTH       1             
#define IN_FM_HEIGHT      28      
#define IN_FM_WIDTH       28       

#define OUT_CONV_FM_DEPTH      1        
#define OUT_CONV_FM_HEIGHT     24       
#define OUT_CONV_FM_WIDTH      24       

#define OUT_MAX_POOL_FM_DEPTH      1        
#define OUT_MAX_POOL_FM_HEIGHT     12       
#define OUT_MAX_POOL_FM_WIDTH      12       

#define IN_LINEAR_LENGTH      144 
#define OUT_LINEAR_LENGTH      10

#define POOL_DIM          2             
#define STRIDE            1             
#define PADDING           0              
#define KERNEL_HEIGHT     5             
#define KERNEL_WIDTH      5  
//--------------------------------------------------------------------------
// Divide the input image into multiple tiles 
//--------------------------------------------------------------------------
#define TILE_HEIGHT       6             
#define TILE_WIDTH        6             

#define N_TILE_ROWS (int) OUT_CONV_FM_HEIGHT / TILE_HEIGHT
#define N_TILE_COLS (int) OUT_CONV_FM_WIDTH  / TILE_WIDTH

#define MARGIN            4

//--------------------------------------------------------------------------
// Input tile buffer dimensions 
//--------------------------------------------------------------------------
#define IN_BUF_DEPTH      1             
#define IN_BUF_HEIGHT     TILE_HEIGHT + MARGIN 
#define IN_BUF_WIDTH      TILE_WIDTH  + MARGIN 

//--------------------------------------------------------------------------
// Output tile buffer dimensions 
//--------------------------------------------------------------------------
#define OUT_BUF_DEPTH     1           
#define OUT_BUF_HEIGHT    TILE_HEIGHT / STRIDE
#define OUT_BUF_WIDTH     TILE_WIDTH  / STRIDE

#define OUT_MAX_POOL_BUF_DEPTH     1             
#define OUT_MAX_POOL_BUF_HEIGHT    3
#define OUT_MAX_POOL_BUF_WIDTH     3

//--------------------------------------------------------------------------
// Top-level Function Declaration
//--------------------------------------------------------------------------

/*void tiled_conv(
    fm_t input_feature_map[IN_FM_DEPTH][IN_FM_HEIGHT][IN_FM_WIDTH],
    wt_t layer_weights[OUT_CONV_FM_DEPTH][IN_FM_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    wt_t linear_weights[IN_LINEAR_LENGTH][OUT_LINEAR_LENGTH],
    fm_t output_feature_map[OUT_LINEAR_LENGTH],
    fm_t target_output[OUT_LINEAR_LENGTH],
    fm_t mse
);*/
void tiled_conv(
    fm_t input_feature_map[IN_FM_DEPTH][IN_FM_HEIGHT][IN_FM_WIDTH],
    wt_t layer_weights[OUT_CONV_FM_DEPTH][IN_FM_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    wt_t linear_weights[IN_LINEAR_LENGTH][OUT_LINEAR_LENGTH],
    fm_t target_output[OUT_LINEAR_LENGTH],
    fm_t mse[1],
    fm_t output_feature_map[OUT_LINEAR_LENGTH]
);

#endif
