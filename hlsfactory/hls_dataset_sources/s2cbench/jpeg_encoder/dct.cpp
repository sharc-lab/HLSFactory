//========================================================================================
// 
// File Name    : dct.cpp
// Description  : JPEG encoder DCT unit
// Release Date : 30/05/2017
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 30/05/2017        1.0        UTD DARClab     JPEG encoder DCT
//=======================================================================================


#define SC_INCLUDE_FX
#include "define.h"
#include "dct.h"

void dct::jpeg_dct(void)
{
  int u,v,x,y;
  int i = 0;
  sc_fixed<18,15,SC_RND,SC_SAT> a;
  sc_int<DCT_OUT_WIDTH> dct_output[8][8];

  sc_fixed<6,2,SC_RND,SC_SAT> dct_co[8][8] = {
    #include "dct_coefficient.dat"
  };
  
  wait();

  while(1)
  {
    //shift 8x8 line buffer by one column
    for(y=0;y<7;y++){
      for(x=0;x<8;x++){
        line_buffer[x][y]=line_buffer[x][y+1];
      }
    }

    //read new data into the line buffer
    for(x=0;x<8;x++)
      line_buffer[x][7] = dct_in[x].read();
    
    i++;
    if(i==8){
      i = 0;
      //DCT
      for(u=0;u<8;u++)
        for(v=0;v<8;v++)
        {
          a = 0;
          for(x=0;x<8;x++)
            for(int y=0;y<8;y++)
              a+= (sc_fixed<18,15,SC_RND,SC_SAT>)line_buffer[x][y]*dct_co[x][u]*dct_co[y][v];
          if(u==0){
            //DC component - 1024, equivalent to all component - 128
            if(v==0) dct_output[u][v] = (0.25*0.5*a - 1024); 
            else dct_output[u][v] = (0.25*0.707*a);
          }
          else {
            if(v==0) dct_output[u][v] = (0.25*0.707*a);
            else dct_output[u][v] = (0.25*a);
          }
        }
      //write outputs
      for(u=0;u<8;u++)
        for(v=0;v<8;v++)
          dct_out[u*8+v].write(dct_output[u][v]);

    }

    wait();
  }
}

