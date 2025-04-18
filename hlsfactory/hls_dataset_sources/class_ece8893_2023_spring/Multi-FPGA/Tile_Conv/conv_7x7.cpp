///////////////////////////////////////////////////////////////////////////////
// Author:      <>
// Course:      ECE8893 - Parallel Programming for FPGAs
// Filename:    conv_7x7.cpp
// Description: Implement a functionally-correct synthesizable 7x7 convolution 
//              for a single tile block without any optimizations
///////////////////////////////////////////////////////////////////////////////
#include "utils.h"

void conv_7x7 (
    fm_t Y_buf[OUT_BUF_DEPTH][OUT_BUF_HEIGHT][OUT_BUF_WIDTH], 
    fm_t X_buf[IN_BUF_DEPTH][IN_BUF_HEIGHT][IN_BUF_WIDTH],
    wt_t W_buf[OUT_BUF_DEPTH][IN_BUF_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    wt_t B_buf[OUT_BUF_DEPTH]
)
{
//---------------------------------------------------------------------------
// Part B: Implement a trivial functionally-correct single-tile convolution.
//
//         This should have an overall latency in the order of 22-23 seconds.
//
//         If it's worse than trivial, it may be worth fixing this first.
//         Otherwise, achieving the target latency with a worse-than-trivial
//         baseline may be difficult!
//
// TODO: Your code for Part B goes here. 
//---------------------------------------------------------------------------
#pragma HLS inline
    
// 1. parallel along OFM depth dimension
// #pragma HLS array_partition variable = X_buf type=cyclic factor=3 dim=1
// #pragma HLS array_partition variable = W_buf type=cyclic factor=3 dim=2

// parallel along filter number
#pragma HLS array_partition variable = W_buf type=cyclic factor=4 dim=1
#pragma HLS array_partition variable = Y_buf type=cyclic factor=4 dim=1
#pragma HLS array_partition variable = B_buf type=cyclic factor=4 dim=1


// 2. parallel along OFM_width depth
#pragma HLS array_partition variable=W_buf type=cyclic factor=7 dim=4
#pragma HLS array_partition variable = Y_buf type=cyclic factor=20 dim=3
#pragma HLS array_partition variable = X_buf type=cyclic factor=45 dim=3



CONV7X7_FILTER_HEIGHT:
for (int fil_h=0; fil_h < KERNEL_WIDTH; fil_h+=1)   // filter height
{
//#pragma HLS pipeline off
		CONV7X7_FILTER_WIDTH:
		for (int fil_w=0; fil_w < KERNEL_WIDTH; fil_w+=1)    // filter width10
		{
//#pragma HLS pipeline off

			CONV7x7_OFB_HEIGHT:
			for (int OFbuffer_h =0; OFbuffer_h < OUT_BUF_HEIGHT; OFbuffer_h+=1)  // OFM_height
			{
//#pragma HLS pipeline off
                CONV7X7_IFB_DEPTH:
                for (int IFbuffer_dep = 0; IFbuffer_dep < IN_BUF_DEPTH; IFbuffer_dep++)    // IFM_depth
                {
// #pragma HLS pipeline
                    CONV7X7_FILTER_NUMBER:
                    for (int filter_num=0; filter_num < OUT_BUF_DEPTH; filter_num++)
                    {
#pragma HLS pipeline
						CONV7X7_OFB_WIDTH:
						for (int OFbuffer_w=0; OFbuffer_w < OUT_BUF_WIDTH; OFbuffer_w+=1)    // OFM_width
						{
								Y_buf[filter_num][OFbuffer_h][OFbuffer_w] +=\
									X_buf[IFbuffer_dep][(OFbuffer_h)*STRIDE +fil_h][(OFbuffer_w)*STRIDE + fil_w] *\
									W_buf[filter_num][IFbuffer_dep][fil_h][fil_w];

						}

					}
				}

		}
	}
}




	Conv_7x7_Add_Bias:
	for (int filter_num=0; filter_num < OUT_BUF_DEPTH; filter_num++)
	{
		for (int OFbuffer_h =0; OFbuffer_h < OUT_BUF_HEIGHT; OFbuffer_h+=1)  // OFM_height
		{

			for (int OFbuffer_w=0; OFbuffer_w < OUT_BUF_WIDTH; OFbuffer_w+=1)    // OFM_width
			{
					Y_buf[filter_num][OFbuffer_h][OFbuffer_w] += B_buf[filter_num];

			}
		}
	}

}
