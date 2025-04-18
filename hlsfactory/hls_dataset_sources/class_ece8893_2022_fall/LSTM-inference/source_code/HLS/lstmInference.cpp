/*
 * Author: Oscar Gao
 */

#include "hls_math.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <ap_fixed.h>
typedef ap_fixed<8,2> FIX_TYPE;

#define SEQ_LENGTH 59
#define NUM_UNITS 5

/**
 * Sigmoid in-house
 *
 * @param x the input
 * @return result of the sigmoid function
 */
FIX_TYPE sigmoid(FIX_TYPE x)
{
	SIGMOID:
    return ( (FIX_TYPE)1/ ((FIX_TYPE)1+ hls::exp((FIX_TYPE)0-x)));
}

/**
 * tanh in-house
 *
 * @param x the input
 * @return result of the tanh function
 */
FIX_TYPE tanh_inhouse(FIX_TYPE x)
{
	TANH:
    return ( (hls::exp(x) - hls::exp((FIX_TYPE)0-x))/(hls::exp(x) + hls::exp((FIX_TYPE)0-x)) );
}

/**
 * Entire LSTM inference function
 *
 * @param inputs the input sequence in [SEQ_LENGTH]
 * @param w_array weight array for inputs in [NUM_UNITS * 4]
 * @param w_bias bias array for inputs in [NUM_UNIT-0.5547, S * 4]
 * @param u_array weight array for hh vector in [NUM_UNITS][NUM_UNITS * 4]
 * @param u_bias bias array for hh vector in [NUM_UNITS * 4]
 * @param dense_weights weights array for the dense layer in [NUM_UNITS]
 * @param dense_bias bias for dense layer
 * @param result the final result written to memory
 *
 */
void lstmInference(FIX_TYPE inputs[SEQ_LENGTH], FIX_TYPE w_array[NUM_UNITS * 4], FIX_TYPE w_bias[NUM_UNITS * 4],
                     FIX_TYPE u_array[NUM_UNITS][NUM_UNITS * 4], FIX_TYPE u_bias[NUM_UNITS * 4],
                     FIX_TYPE dense_weights[NUM_UNITS], FIX_TYPE dense_bias[1], FIX_TYPE result[1]) {

	// declare ports -- parameterize later
	#pragma HLS interface m_axi port=inputs   	    depth=59
	#pragma HLS interface m_axi port=w_array 	    depth=(5*4)
	#pragma HLS interface m_axi port=w_bias 	    depth=(5*4)
	#pragma HLS interface m_axi port=u_array	    depth=(5*5*4)
	#pragma HLS interface m_axi port=u_bias         depth=(5*4)
	#pragma HLS interface m_axi port=dense_weights  depth=5
	#pragma HLS interface m_axi port=dense_bias     depth=1
	#pragma HLS interface m_axi port=result         depth=1
	#pragma HLS interface s_axilite register port=return

#pragma HLS latency

    // Read in the data from DRAM to BRAM
	FIX_TYPE inputs_B[SEQ_LENGTH];
	FIX_TYPE w_array_B[NUM_UNITS * 4];
	FIX_TYPE w_bias_B[NUM_UNITS * 4];
	FIX_TYPE u_array_B[NUM_UNITS][NUM_UNITS * 4];
	FIX_TYPE u_bias_B[NUM_UNITS * 4];
	FIX_TYPE dense_weights_B[NUM_UNITS];
	FIX_TYPE dense_bias_B[1];

	// Read in the data from DRAM to BRAM
	READ_IN:
	for(int i = 0; i < SEQ_LENGTH; i++)
	{
		inputs_B[i] = inputs[i];
	}

	READ_W_ARRAY:
	for(int i = 0; i < NUM_UNITS*4; i++)
	{
		w_array_B[i] = w_array[i];
	}

	READ_W_BIAS:
	for(int i = 0; i < NUM_UNITS*4; i++)
	{
		w_bias_B[i] = w_bias[i];
	}

	READ_U_ARRAY:
	for(int i = 0; i < NUM_UNITS; i++)
	{
		for(int j = 0; j < NUM_UNITS*4; j++)
		{
			u_array_B[i][j] = u_array[i][j];
	    }
	}

	READ_U_BIAS:
	for(int i = 0; i < NUM_UNITS*4; i++)
	{
		u_bias_B[i] = u_bias[i];
	}

	READ_DENSE_WEIGHTS:
	for(int i = 0; i < NUM_UNITS; i++)
	{
		dense_weights_B[i] = dense_weights[i];
	}


	// temporary variables -- initialized to 0
	FIX_TYPE hidden_states[NUM_UNITS] = {0};
	FIX_TYPE cell_states[NUM_UNITS] = {0};
	FIX_TYPE prev_hidden_states[NUM_UNITS] = {0};

//#pragma HLS ARRAY_PARTITION variable=u_array_B type=complete // for some reason, u_array_B needs to be partitioned

	// LSTM layer
	SEQ:
	for (int j = 0; j < SEQ_LENGTH; j++)
	{
		UNITS:
		for (int i = 0; i < NUM_UNITS; i++)
		{
#pragma HLS PIPELINE II=1
			FIX_TYPE f = inputs_B[j] * w_array_B[NUM_UNITS * 1 + i];
			F:
	        for (int k = 0; k < NUM_UNITS; k++)
	        {
	        	f += prev_hidden_states[k] * u_array_B[k][NUM_UNITS * 1 + i];
	        }
	        f += (w_bias_B[NUM_UNITS * 1 + i] + u_bias_B[NUM_UNITS * 1 + i]);
	        FIX_TYPE f_r = sigmoid(f);


	        FIX_TYPE i_input = inputs_B[j] * w_array_B[i];
	        I:
	        for (int k = 0; k < NUM_UNITS; k++)
	        {
	        	i_input += prev_hidden_states[k] * u_array_B[k][i];
	        }
	        i_input += (w_bias_B[i] + u_bias_B[i]);
	        FIX_TYPE i_r = sigmoid(i_input);


	        FIX_TYPE c = inputs_B[j] * w_array_B[NUM_UNITS * 2 + i];
	        C:
			for (int k = 0; k < NUM_UNITS; k++)
			{
	        	c += prev_hidden_states[k] * u_array_B[k][NUM_UNITS * 2 + i];
	        }
	        c += (w_bias_B[NUM_UNITS * 2 + i] + u_bias_B[NUM_UNITS * 2 + i]);

	        cell_states[i] = f_r * cell_states[i] + tanh_inhouse(c) * i_r;


	        FIX_TYPE o = inputs_B[j] * w_array_B[NUM_UNITS * 3 + i];
	        O:
	        for (int k = 0; k < NUM_UNITS; k++)
	        {
	        	o += prev_hidden_states[k] * u_array_B[k][NUM_UNITS * 3 + i];
	        }
	        o += (w_bias_B[NUM_UNITS * 3 + i] + u_bias_B[NUM_UNITS * 3 + i]);
	        FIX_TYPE o_r = sigmoid(o);

	        hidden_states[i] = o_r * tanh_inhouse(cell_states[i]);
		}

	    // update hidden states
		HID:
	    for (int i = 0; i < NUM_UNITS; i++)
	    {
#pragma HLS PIPELINE
	    	prev_hidden_states[i] = hidden_states[i];
	    }
	}

	// Fully-Connected Dense Layer
	result[0] = dense_bias[0];
	DENSE:
	for (int i = 0; i < NUM_UNITS; i++)
	{
#pragma HLS PIPELINE
		result[0] += dense_weights_B[i]*hidden_states[i];
	}
}

