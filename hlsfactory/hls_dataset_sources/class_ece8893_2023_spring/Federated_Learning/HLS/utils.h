///////////////////////////////////////////////////////////////////////////////
// Course:      ECE8893 - Parallel Programming for FPGAs
// Filename:    utils.h
// Description: Header file for utility functions.
///////////////////////////////////////////////////////////////////////////////

#include "conv.h"

//--------------------------------------------------------------------------
// Function Declarations
//--------------------------------------------------------------------------
void load_input_tile_block_from_DRAM (
    fm_t in_fm_buf[IN_BUF_DEPTH][IN_BUF_HEIGHT][IN_BUF_WIDTH], 
    fm_t in_fm[IN_FM_DEPTH][IN_FM_HEIGHT][IN_FM_WIDTH], 
    int  ti, 
    int  tj 
);

void load_layer_params_from_DRAM(
    wt_t weight_buf[OUT_BUF_DEPTH][IN_BUF_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    wt_t weights[OUT_CONV_FM_DEPTH][IN_FM_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    int  kernel_group
);

void conv_5x5(
    fm_t Y_buf[OUT_BUF_DEPTH][OUT_BUF_HEIGHT][OUT_BUF_WIDTH],
    fm_t X_buf[IN_BUF_DEPTH][IN_BUF_HEIGHT][IN_BUF_WIDTH],
    wt_t W_buf[OUT_BUF_DEPTH][IN_BUF_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH]
);

void max_pool(
    fm_t conv_in_buf[OUT_BUF_DEPTH][OUT_BUF_HEIGHT][OUT_BUF_WIDTH],
    fm_t max_pool_out_buf[OUT_MAX_POOL_BUF_DEPTH][OUT_MAX_POOL_BUF_HEIGHT][OUT_MAX_POOL_BUF_WIDTH],
    int dim
);

void store_output_tile_to_DRAM (
    fm_t conv_out_fm[OUT_MAX_POOL_FM_DEPTH][OUT_MAX_POOL_FM_HEIGHT][OUT_MAX_POOL_FM_WIDTH],
    fm_t out_fm_buf[OUT_MAX_POOL_BUF_DEPTH][OUT_MAX_POOL_BUF_HEIGHT][OUT_MAX_POOL_BUF_WIDTH],
    int  ti,
    int  tj,
    int  kernel_group
);

void quarter_drop(
    fm_t max_pool_out_buf[OUT_MAX_POOL_BUF_DEPTH][OUT_MAX_POOL_BUF_HEIGHT][OUT_MAX_POOL_BUF_WIDTH]
);

unsigned int pseudo_random(
    unsigned int seed, 
    int load);

void linear_layer(
    fm_t linear_input[OUT_MAX_POOL_FM_DEPTH * OUT_MAX_POOL_FM_HEIGHT * OUT_MAX_POOL_FM_WIDTH],
    wt_t linear_weights[IN_LINEAR_LENGTH][OUT_LINEAR_LENGTH],
    fm_t output_feature_map[OUT_LINEAR_LENGTH]
);

void softmax(
    fm_t output_feature_map[OUT_LINEAR_LENGTH],
    fm_t softmax_output[OUT_LINEAR_LENGTH]
);

void backprop(
    fm_t linear_input[OUT_MAX_POOL_FM_DEPTH * OUT_MAX_POOL_FM_HEIGHT * OUT_MAX_POOL_FM_WIDTH],
    wt_t linear_weights[IN_LINEAR_LENGTH][OUT_LINEAR_LENGTH],
    fm_t output_feature_map[OUT_LINEAR_LENGTH], //This is output of softmax
    fm_t target_output[OUT_LINEAR_LENGTH]
);

void calculateMSE(
    fm_t target_output[OUT_LINEAR_LENGTH],
    fm_t output_feature_map[OUT_LINEAR_LENGTH],
    fm_t mse[1]
);
