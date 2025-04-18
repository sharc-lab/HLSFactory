///////////////////////////////////////////////////////////////////////////////
// Course:      ECE8893 - Parallel Programming for FPGAs
// Filename:    utils.cpp
// Description: Utility functions to implement tiling-based convolution
//
// Note:        Modify/complete the functions as required by your design.
//
//              You are free to create any additional functions you may need
//              for your implementation.
///////////////////////////////////////////////////////////////////////////////
#include "utils.h"
#include <bitset>
#include <string>
//--------------------------------------------------------------------------
// Function to load an input tile block from from off-chip DRAM
// to on-chip BRAM.
//
// TODO: This is an incomplete function that you need to modify
//       to handle the border conditions appropriately while loading.
//--------------------------------------------------------------------------
void load_input_tile_block_from_DRAM(
    fm_t in_fm_buf[IN_BUF_DEPTH][IN_BUF_HEIGHT][IN_BUF_WIDTH],
    fm_t in_fm[IN_FM_DEPTH][IN_FM_HEIGHT][IN_FM_WIDTH],
    int ti,
    int tj)
{
#pragma HLS inline

    const int P = PADDING;

    const int height_offset = ti * TILE_HEIGHT;
    const int width_offset = tj * TILE_WIDTH;

// INPUT_BUFFER_DEPTH:
// for(int c = 0; c < IN_BUF_DEPTH; c++)       // channel
// {
//     INPUT_BUFFER_HEIGHT:
//     for(int i = 0; i < IN_BUF_HEIGHT; i++)  // i -> height
//     {
//         INPUT_BUFFER_WIDTH:
//         for(int j = 0; j < IN_BUF_WIDTH; j++)   // j-> width
//         {
//             // TODO: Handle border features here
//             //
//             // Hint: Either load 0 or input feature into
//             //       the buffer based on border conditions

//             in_fm_buf[f][i][j] = 0; // Just a placeholder
//         }
//     }
// }
INPUT_BUFFER_DEPTH:
    for (int c = 0; c < IN_BUF_DEPTH; c++)
    {
    INPUT_BUFFER_HEIGHT:
        for (int i = 0; i < IN_BUF_HEIGHT; i++)
        {
        INPUT_BUFFER_WIDTH:
            for (int j = 0; j < IN_BUF_WIDTH; j++)
            {
                if (((ti == 0) && (i < P)) ||
                    ((tj == 0) && (j < 3)) ||
                    ((ti == N_TILE_ROWS - 1) && ((IN_BUF_HEIGHT - 1 - i) < 2)) ||
                    ((tj == N_TILE_COLS - 1) && ((IN_BUF_WIDTH - j - 1) < 2)))
                    in_fm_buf[c][i][j] = 0;
                else
                {
                    in_fm_buf[c][i][j] = in_fm[c][height_offset - P + i][width_offset - P + j];
                }
            }
        }
    }
}

//--------------------------------------------------------------------------
// Function to load layer parameters (weights and bias) for convolution.
//
// This function can be used as-is in your design. However, you are free to
// add pragmas, restructure code, etc. depending on your way of optimization.
//--------------------------------------------------------------------------
void load_layer_params_from_DRAM(
    wt_t weight_buf[OUT_BUF_DEPTH][IN_BUF_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    wt_t bias_buf[OUT_BUF_DEPTH],
    wt_t weights[OUT_FM_DEPTH][IN_FM_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    wt_t bias[OUT_FM_DEPTH],
    int kernel_group)
{
#pragma HLS inline

    const int kernel_offset = kernel_group * OUT_BUF_DEPTH;

WEIGHT_KERNEL_NUM:
    for (int f = 0; f < OUT_BUF_DEPTH; f++)
    {
    WEIGHT_KERNEL_DEPTH:
        for (int c = 0; c < IN_BUF_DEPTH; c++)
        {
        WEIGHT_KERNEL_HEIGHT:
            for (int kh = 0; kh < 7; kh++)
            {
            WEIGHT_KERNEL_WIDTH:
                for (int kw = 0; kw < 7; kw++)
                {
                    weight_buf[f][c][kh][kw] = weights[kernel_offset + f][c][kh][kw];
                }
            }
        }
    }

BIAS:
    for (int f = 0; f < OUT_BUF_DEPTH; f++)
    {
        bias_buf[f] = bias[kernel_offset + f];
    }
}

//--------------------------------------------------------------------------
// Function to store complete output tile block from BRAM to DRAM.
//
// In-place ReLU has been incorporated for convenience.
//
// This function can be used as-is in your design. However, you are free to
// add pragmas, restructure code, etc. depending on your way of optimization.
//--------------------------------------------------------------------------

int limit = 10;
void store_output_tile_to_DRAM(
    hls::stream<ap_axis<16, 1, 1, 1>> &out_fm,
    fm_t out_fm_buf[OUT_BUF_DEPTH][OUT_BUF_HEIGHT][OUT_BUF_WIDTH],
    int ti,
    int tj,
    int kernel_group)
{
#pragma HLS inline

    const int depth_offset = kernel_group * OUT_BUF_DEPTH;
    const int height_offset = ti * OUT_BUF_HEIGHT;
    const int width_offset = tj * OUT_BUF_WIDTH;
    
OUTPUT_BUFFER_DEPTH:
    for (int f = 0; f < OUT_BUF_DEPTH; f++)
    {
    OUTPUT_BUFFER_HEIGHT:
        for (int i = 0; i < OUT_BUF_HEIGHT; i++)
        {
        OUTPUT_BUFFER_WIDTH:
            for (int j = 0; j < OUT_BUF_WIDTH; j++)
            {
                // ReLU in-place
                if (out_fm_buf[f][i][j] < (fm_t)0)
                {
                    ap_axis<16, 1, 1, 1> tmp;
                    tmp.data = 0;
                    tmp.last = 0;
                    tmp.user = 0;
                    tmp.id = 0;
                    out_fm.write(tmp);
                }
                else
                {
                    ap_axis<16, 1, 1, 1> tmp;
                    ap_fixed<16,3> fp2;
                    fp2 = out_fm_buf[f][i][j];
                    // fp2 = fp2 * 500;
                    ap_int<16> raw2;
                    for(int t=0;t<16;++t){
                        raw2[t] = fp2[t];
                    }
                    tmp.data =raw2;
                    if (limit > 0)
                    {
                        // for(int t=0;t<16;++t){
                        //     std::cout << fp2[t];
                        // }
                        // std::cout << std::endl;
                        // for(int t=0;t<16;++t){
                        //     std::cout << tmp.data[t];
                        // }
                        // std::cout << std::endl;

                        // std::cout << "writing No. "<< 10 - limit << ": " <<out_fm_buf[f][i][j] << std::endl;
                        // std::cout << tmp.data << std::endl;
                        limit--;
                    }
                    
                    // std::cout << "writing " <<out_fm_buf[f][i][j] << " becomes ";
                    // std::cout << tmp.data << std::endl;

                    // ap_int<16> raw2;
                    // ap_fixed<16,3> fp2;
                    // raw2 = tmp.data;
                    // fp2 = ap_fixed<16,3> (&raw2);
                    // std::cout << "fp2: " << fp2 << std::endl;
                    // std::cout << "raw2s: " << raw2 << std::endl;
                    tmp.last = 0;
                    tmp.user = 0;
                    tmp.id = 0;
                    out_fm.write(tmp);
                }
                out_fm_buf[f][i][j] = 0;
            }
        }
    }
}
