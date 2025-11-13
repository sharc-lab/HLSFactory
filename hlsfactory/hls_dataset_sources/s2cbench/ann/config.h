//========================================================================================
// 
// File Name    : config.h
// Description  : 
// Release Date : 22/10/2014
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author             Description
//----------------------------------------------------------------------------------------
// 2014          1.0            David Aledo        ANN configuration (2 layers)
// 2014          1.1            B Carrion Schafer  added 2 and 4 layer configuration
//
//=======================================================================================

#ifndef CONFIG_H
#define CONFIG_H

//#define NLAYER_4  // Uncomment for 4 layer version ANN
//#define PRINTD  // To print extra data like weights and bias



#define NumI 16 // Number of inputs
#define NumO 16 // Number of outputs
#define baseB 4 // Sqrt of NumI. It represents the side of the patches which the image is divided into

#define MAX_N 16

#ifdef NLAYER_4
	#define Nlayer 4
	const int NumN[Nlayer+1]={16,12,8,12,16};//4-layer+1 output layer. Editable
#else
	#define Nlayer 2
	const int NumN[Nlayer+1]={16,8,16};     // 2-Layer+1 output layer Editable
#endif


template <const int N_In, const int N_N>
struct synth_layer_data
{
   int W[N_N][N_In];
   int b[N_N];
};


/* Layer types definitions */
#ifdef NLAYER_4
   typedef synth_layer_data<16,12> type_0;
   typedef synth_layer_data<12,8> type_1;
   typedef synth_layer_data<8,12> type_2;
   typedef synth_layer_data<12,16> type_3;
#else
   typedef synth_layer_data<16,8> type_0;
   typedef synth_layer_data<8,16> type_1;
#endif

#endif
