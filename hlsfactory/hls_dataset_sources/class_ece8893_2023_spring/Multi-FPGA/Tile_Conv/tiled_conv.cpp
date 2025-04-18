///////////////////////////////////////////////////////////////////////////////
// Author:      <>
// Course:      ECE8893 - Parallel Programming for FPGAs
// Filename:    tiled_conv.cpp
// Description: Implement a functionally-correct synthesizable tiling-based 
//              convolution for ResNet-50's first 7x7 layer with an 
//              HD input image.
//              
// Note: Do not use pragmas other than the existing ones in Part B.
///////////////////////////////////////////////////////////////////////////////
#include "utils.h"
#include <cstring>

#define HLS_AUTO_PIPELINE_OFF
#define HLS_AUTO_ARRAY_PARTITION_OFF

void tiled_conv (
    fm_t input_feature_map[IN_FM_DEPTH][IN_FM_HEIGHT][IN_FM_WIDTH],
    wt_t layer_weights[OUT_FM_DEPTH][IN_FM_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    wt_t layer_bias[OUT_FM_DEPTH],
    hls::stream <ap_axis<16,1,1,1>> &output_feature_map
)
{
    //--------------------------------------------------------------------------
    // Defines interface IO ports for HLS. 
    //--------------------------------------------------------------------------
    #pragma HLS INTERFACE m_axi depth=1  port=input_feature_map   bundle=ip
    #pragma HLS INTERFACE m_axi depth=1  port=layer_weights       bundle=ip
    #pragma HLS INTERFACE m_axi depth=1  port=layer_bias          bundle=ip
    #pragma HLS INTERFACE axis depth=1  port=output_feature_map
    
    // #pragma HLS INTERFACE s_axilite port=output_feature_map bundle=control

    #pragma HLS INTERFACE s_axilite register port=return bundle=control
    
    //--------------------------------------------------------------------------
    // On-chip buffers
    // You should NOT modify the buffer dimensions!
    //--------------------------------------------------------------------------
    fm_t conv_in_buf[IN_BUF_DEPTH][IN_BUF_HEIGHT][IN_BUF_WIDTH];
    wt_t conv_wt_buf[OUT_BUF_DEPTH][IN_BUF_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH];
    wt_t conv_bias_buf[OUT_BUF_DEPTH];
    fm_t conv_out_buf[OUT_BUF_DEPTH][OUT_BUF_HEIGHT][OUT_BUF_WIDTH] = {0};
    
    //--------------------------------------------------------------------------
    // Process each tile iteratively
    //--------------------------------------------------------------------------
    //fm_t num_slice =  OUT_FM_DEPTH / OUT_BUF_DEPTH;
    //fm_t num_slice =  16;
    
    int num_slice=16;
    TILE_ROW:
    for(int ti = 0; ti < N_TILE_ROWS; ti++)
    {

		TILE_SLICE:
		for(int tj = 0; tj < N_TILE_COLS; tj+=1)
        {
//            std::cout << "Processing Tile " << ti*N_TILE_COLS + tj + 1;
//            std::cout << "/" << N_TILE_ROWS * N_TILE_COLS << std::endl;

            //--------------------------------------------------------------------------
            // TODO: Your code for Task B and Task C goes here 
            //
            // Implement the required code to run convolution on an entire tile. 
            // Refer to utils.cpp for the required functions
            //
            // Hint: You need to split the filter kernels into sub-groups 
            //       for processing.
            //--------------------------------------------------------------------------
            load_input_tile_block_from_DRAM(conv_in_buf, input_feature_map, ti, tj);
                
			TILE_COL:
			for (int ts =0; ts < num_slice; ts ++)
            {

                load_layer_params_from_DRAM(conv_wt_buf, conv_bias_buf, layer_weights, layer_bias, ts);

                conv_7x7(conv_out_buf, conv_in_buf, conv_wt_buf, conv_bias_buf);

                store_output_tile_to_DRAM(output_feature_map, conv_out_buf, ti, tj, ts);


// memset(conv_out_buf, 0, sizeof(conv_out_buf));

            }
        }
    }

}
