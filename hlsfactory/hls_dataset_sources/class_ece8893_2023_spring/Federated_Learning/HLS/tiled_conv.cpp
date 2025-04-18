///////////////////////////////////////////////////////////////////////////////
// Course:      ECE8893 - Parallel Programming for FPGAs
// Filename:    tiled_conv.cpp
// Description: Implements a synthesizable tiling-based 
//              convolution, max-pooling followed by fully-connected
//              layer, softmax, backpropagation and MSE calculation.
///////////////////////////////////////////////////////////////////////////////
#include "utils.h"
#include <iostream>
#include <ap_fixed.h>

using namespace std;

void tiled_conv(
    fm_t input_feature_map[IN_FM_DEPTH][IN_FM_HEIGHT][IN_FM_WIDTH],
    wt_t layer_weights[OUT_CONV_FM_DEPTH][IN_FM_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    wt_t linear_weights[IN_LINEAR_LENGTH][OUT_LINEAR_LENGTH],
    fm_t target_output[OUT_LINEAR_LENGTH],
    fm_t mse[1],
    fm_t output_feature_map[OUT_LINEAR_LENGTH]
)
{
    //--------------------------------------------------------------------------
    // Defines interface IO ports for HLS. 
    //--------------------------------------------------------------------------

    #pragma HLS INTERFACE m_axi depth=1  port=input_feature_map   bundle=fm
    #pragma HLS INTERFACE m_axi depth=1  port=layer_weights       bundle=wt
    #pragma HLS INTERFACE m_axi depth=1  port=linear_weights      bundle=wt
    #pragma HLS INTERFACE m_axi depth=1  port=target_output       bundle=fm
    #pragma HLS INTERFACE m_axi depth=1  port=mse       bundle=fm
    #pragma HLS INTERFACE m_axi depth=1  port=output_feature_map  bundle=fm

    #pragma HLS INTERFACE s_axilite register	port=return
    
    //--------------------------------------------------------------------------
    // On-chip buffers
    // You should NOT modify the buffer dimensions!
    //--------------------------------------------------------------------------
    fm_t conv_in_buf[IN_BUF_DEPTH][IN_BUF_HEIGHT][IN_BUF_WIDTH];
    wt_t conv_wt_buf[OUT_BUF_DEPTH][IN_BUF_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH];
    fm_t conv_out_buf[OUT_BUF_DEPTH][OUT_BUF_HEIGHT][OUT_BUF_WIDTH] = { 0 };
    fm_t max_pool_out_buf[OUT_MAX_POOL_BUF_DEPTH][OUT_MAX_POOL_BUF_HEIGHT][OUT_MAX_POOL_BUF_WIDTH];
    fm_t layer1_output[OUT_MAX_POOL_FM_DEPTH][OUT_MAX_POOL_FM_HEIGHT][OUT_MAX_POOL_FM_WIDTH];
    fm_t linear_input[OUT_MAX_POOL_FM_DEPTH * OUT_MAX_POOL_FM_HEIGHT * OUT_MAX_POOL_FM_WIDTH];
    fm_t inferred_feature_map[OUT_LINEAR_LENGTH];
    fm_t softmax_output[OUT_LINEAR_LENGTH]; 
    //fm_t output_feature_map[OUT_LINEAR_LENGTH];
    
    //#pragma HLS array_partition variable=conv_out_buf dim=1 complete
    //#pragma HLS array_partition variable=conv_in_buf dim=1 complete
    //#pragma HLS array_partition variable=conv_wt_buf dim=1 complete

    //--------------------------------------------------------------------------
    // Process each tile iteratively
    //--------------------------------------------------------------------------
    TILE_ROW:
    for(int ti = 0; ti < N_TILE_ROWS; ti++)
    {
        TILE_COL:
        for(int tj = 0; tj < N_TILE_COLS; tj++)
        {
            std::cout << "Processing Tile " << ti * N_TILE_COLS + tj + 1;
            std::cout << "/" << N_TILE_ROWS * N_TILE_COLS << std::endl;    

            load_input_tile_block_from_DRAM(conv_in_buf, input_feature_map, ti, tj);

            for (int i = 0; i < OUT_CONV_FM_DEPTH / OUT_BUF_DEPTH; i++) { 
                load_layer_params_from_DRAM(conv_wt_buf, layer_weights, i);
                conv_5x5(conv_out_buf, conv_in_buf, conv_wt_buf);
                max_pool(conv_out_buf, max_pool_out_buf, POOL_DIM);
                store_output_tile_to_DRAM(layer1_output, max_pool_out_buf, ti, tj, i);
            }
        }

        for (int td = 0; td < OUT_MAX_POOL_BUF_DEPTH; td++) {
            for (int tp = 0; tp < OUT_MAX_POOL_BUF_HEIGHT; tp++) {
                int row_no = ti * OUT_MAX_POOL_BUF_HEIGHT + tp;
                for (int tj = 0; tj < OUT_MAX_POOL_FM_WIDTH; tj++) {
                    linear_input[row_no * OUT_MAX_POOL_FM_WIDTH + tj] = layer1_output[td][row_no][tj];
                    //std::cout << linear_input[row_no * OUT_MAX_POOL_FM_WIDTH + tj] << std::endl;
                }
	    }
        }
    }

    cout<<"\nOutput feature map:"<<endl;
    linear_layer(linear_input, linear_weights, inferred_feature_map);
    softmax(inferred_feature_map, softmax_output);
    for (int i = 0; i < OUT_LINEAR_LENGTH; i++) {
        output_feature_map[i] = softmax_output[i];
	//std::cout << "Softmax output" << std::endl;
	//std::cout << output_feature_map[i] << std::endl;
    }
    //backprop(linear_input, linear_weights, output_feature_map, target_output);
    backprop(linear_input, linear_weights, softmax_output, target_output);
    calculateMSE(target_output, softmax_output, mse);
}
