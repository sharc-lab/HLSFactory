//========================================================================================
// 
// File Name    : rle.cpp
// Description  : JPEG RLE 
// Release Date : 26/02/2018
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 30/05/2017        1.0        UTD DARClab     JPEG RLE
// 26/02/2018        1.1        UTD DARCLab     fixed a small bug; use index table for zigzag
//=======================================================================================


#include "rle.h"

void rle::jpeg_rle()
{
  sc_int<QUAN_OUT_WIDTH> quantized_block[8][8];
  sc_int<QUAN_OUT_WIDTH> rl[RLE_BUFF_LEN];
  sc_uint<8> len; //length of RL code
  int x,y,i,k;
  previous_dc = 0;
  int j = -2;

  wait();

  while(1)
  {
    j++;
    if(j==8){
      j = 0;
      i = 1;
      k = 0;
      len = 1;
      // read inputs
      for(x=0;x<8;x++)
        for(y=0;y<8;y++)
          quantized_block[x][y] = rle_in[x*8+y].read();

      // zigzag
      jpeg_zigzag(quantized_block);
  
      // run length encoding
      // DC component
      rl[0] = zz[0] - previous_dc;
      previous_dc = zz[0];
  
      // AC components 
      while(i<64)
      {
        k=0;
        while((i<64)&&(zz[i]==0)&&(k<15))
        {
          i++;
          k++;
        }
        if(i==64)  //end of block
        {
          rl[len++] = 0;
          rl[len++] = 0;
        }
        else
        {
          rl[len++] = k; //number of zeros before a non-zero number
          rl[len++] = zz[i++];  //non-zero number
        }
      }

      while((rl[len-4]==15)&&(rl[len-3]==0))
      {
          rl[len-4]=0;
          len-=2;
      }
      //write outputs
      for(x=0;x<RLE_BUFF_LEN;x++)
          rle_out[x].write(rl[x]);
  
      rle_length.write(len);
    }
    wait();
  }

}

void rle::jpeg_zigzag(sc_int<QUAN_OUT_WIDTH> input[8][8])
{
  int i;
  int x_index[64] = {0,0,1,2,1,0,0,1,2,3,4,3,2,1,0,0,1,2,3,4,5,6,5,4,3,2,1,0,0,1,2,3,
                     4,5,6,7,7,6,5,4,3,2,1,2,3,4,5,6,7,7,6,5,4,3,4,5,6,7,7,6,5,6,7,7};
  int y_index[64] = {0,1,0,0,1,2,3,2,1,0,0,1,2,3,4,5,4,3,2,1,0,0,1,2,3,4,5,6,7,6,5,4,
                     3,2,1,0,1,2,3,4,5,6,7,7,6,5,4,3,2,3,4,5,6,7,7,6,5,4,5,6,7,7,6,7};

  for( i=0; i<64; i++ )
	  zz[i] = input[x_index[i]][y_index[i]];

}
