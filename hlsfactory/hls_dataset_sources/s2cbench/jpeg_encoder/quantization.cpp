//========================================================================================
// 
// File Name    : quantization.cpp
// Description  : JPEG encoder quantization unit
// Release Date : 30/05/2017
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 30/05/2017        1.0        UTD DARClab     JPEG encoder quantization
//=======================================================================================


#include "quantization.h"

void quantization::jpeg_quantization()
{
  //quantization matrix
  sc_int<8> Q[8][8] =  {
    #include "quantization_matrix.dat"
  };
  sc_int<DCT_OUT_WIDTH> dct_block[8][8];
  sc_int<QUAN_OUT_WIDTH> quantization_output[8][8];
  int i,j;
  int k = -1;

  wait();

  while(1)
  {
    k++;
    if(k==8){
      k = 0;
      //read inputs
      for(i=0;i<8;i++)
        for(j=0;j<8;j++)
          dct_block[i][j] = quantization_in[i*8+j].read();

      //quantizatioin
      for(i=0;i<8;i++)
        for(j=0;j<8;j++)
          quantization_output[i][j] = dct_block[i][j]/Q[i][j];

      //write outputs
      for(i=0;i<8;i++)
        for(j=0;j<8;j++)
          quantization_out[i*8+j].write(quantization_output[i][j]);  
    }

    wait();
  }
}
