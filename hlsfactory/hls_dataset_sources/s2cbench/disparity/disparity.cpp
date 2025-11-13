//========================================================================================
// 
// File Name    : disparity.cpp
// Description  : disparity estimator
// Release Date : 23/02/2018
// Author       : PolyU, UT Dallas DARClab
//                Shuangnan Liu, Jianqi Chen, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 23/02/2018    1.0           DARClab          disparity main description
//========================================================================================

#include "disparity.h"

// right shift the right image and calculate the difference 
void disparity::shift_diff( sc_uint<8> image_in_r[IN_BUFFER_DEPTH][WIN_SIZE],
	sc_uint<8> image_in_l[IN_BUFFER_DEPTH][WIN_SIZE], sc_uint<16> width, int shift)
{
	int i         = 0;
	int j         = 0;
	int diff      = 0;

	for (j = 0; j < WIN_SIZE; j++)
	{
		for (i = 0; i < width ; i++)
		{
			if (i<shift)
				image_diff[i][j] = 0;
			if (i < width - shift )
				image_diff[i+shift][j] = image_in_r[i][j];
			
			diff = image_in_l[i][j] - image_diff[i][j];
			if (diff < 0)
				diff = - diff;
			if (diff > 255)
				diff = 255;
			image_diff[i][j] = (sc_uint<8>)(diff);
		}
	}
}

// calculate the accumulated difference of a window
void disparity::window_sad(sc_uint<16> width)
{
	int j     = 0;
	int i     = 0;
	int ii    = 0;
	int accu  = 0;

	for (i = 0; i < width - WIN_SIZE + 1; i++)
	{
		accu = 0;

		for (j = 0 ; j < WIN_SIZE ; j++)
		{
			for (ii = 0; ii < WIN_SIZE; ii++)
			{
				accu += image_diff[i + ii][j];
			}
		}
		image_win_sad[i] = (accu > 0xFF) ? 0xFF: accu;
	}
}

// find the number of pixels shifted of the most matched shifted image
void disparity::find_disparity(sc_uint<16> width, int shift)
{
	int i     = 0;

	for (i=0; i < width - WIN_SIZE + 1; i++)
	{
		if (image_win_sad[i] < image_min_sad[i])
		{
			image_min_sad[i] = image_win_sad[i];
			output_buffer[i] = shift * (256/MAX_SHIFT);
		}
	}
}

// main function of CThread
void disparity::disparity_main(void)
{
	int i, j;
	int shift;
	
	wait();

	while(1){
		// read inputs
		ready.write(0);

		wait();
		width_read = width.read();
		for ( i=0; i < width_read; i++){
			//shift
			for ( j=WIN_SIZE-1; j>0; j--){
				in_buffer_r[i][j] = in_buffer_r[i][j-1];
				in_buffer_l[i][j] = in_buffer_l[i][j-1];
			}
			// read inputs
			in_buffer_r[i][0] = data_in_r.read();
			in_buffer_l[i][0] = data_in_l.read();

			wait();
		}
		
		//initialize image_min_sad
		for ( i=0; i<IN_BUFFER_DEPTH; i++){
			image_min_sad[i] = 255;
		}
		// calculate disparity
		for (shift = 0; shift < MAX_SHIFT; shift++)
		{
			shift_diff(in_buffer_r, in_buffer_l, width_read, shift);
			window_sad(width_read);
			find_disparity(width_read, shift);
		}
		
		// write outputs
		for ( i=0; i < width_read - WIN_SIZE + 1; i++){
			data_out.write(output_buffer[i]);
			ready.write(1);
			wait();
		}
	
	}
}
