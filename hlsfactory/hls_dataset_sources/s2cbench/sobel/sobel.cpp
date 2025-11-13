//========================================================================================
// 
// File Name    : sobel.h
// Description  : Sobel filter implemenetation
// Release Date : 23/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version    Author            Description
//---------------------------------------------------------------------------------------
//23/07/2013      1.0     PolyU DARC Lab     main sobel definition header         
//=======================================================================================
#include "define.h"
#include "sobel.h"


void sobel::sobel_main(void)
{

  sc_uint<8> input_row_read[3];
  sc_uint<8> output_row_write;
  int i,j,x;


  wait();
 
  while(1)
  {

      // Reading triplet data needed for filter

    input_row_read[0] = input_row[0].read();
    input_row_read[1] = input_row[1].read();
    input_row_read[2] = input_row[2].read();

    // Perform the filtering of a 3x3 pixel image segment
    output_row_write =  sobel_filter(input_row_read); 
    // Writting filtered output back 
    output_row.write(output_row_write);
    wait();
  }// end of while
}


//
// Sobel filter function
//
sc_uint<8>  sobel::sobel_filter(sc_uint<8> *input_row_r)
{

  unsigned int X, Y;
  unsigned char orow;
  int sumX, sumY;
  int SUM, rowOffset, colOffset;


  
  int Gx[3][3] ={{1 ,0 ,-1},
		  { 2, 0, -2},
		  { 1, 0, -1}};  


  int Gy[3][3] ={{1, 2, 1},
		  {0, 0, 0},
		  {-1, -2, -1}};


  /* Shifting 3x3 line buffer by one row  */

   for(Y=2;Y>0;Y--){
     for(X=0;X< 3;X++){
       line_buffer[X][Y-1]=line_buffer[X][Y];
     }
   }	
	
   // Reading new data into the line buffer
   for(X=0; X<SIZE_BUFFER; X++)
     line_buffer[X][2] = input_row_r[X];

 
   sumX = 0;
   sumY = 0;

   // Convolution starts here
   //-------X GRADIENT APPROXIMATION------
   //-------Y GRADIENT APPROXIMATION------   
   for(rowOffset = -1; rowOffset <= 1; rowOffset++){
     for(colOffset = -1; colOffset <=1; colOffset++){
       sumX = sumX + line_buffer[1 +rowOffset][1-colOffset] * Gx[1+rowOffset][1+colOffset];
       sumY = sumY + line_buffer[1 +rowOffset][1-colOffset] * Gy[1+rowOffset][1+colOffset];
   	}
    }
      if(sumX < 0)      sumX = -sumX;
      if(sumX > 255)    sumX = 255;

	  if(sumY < 0)      sumY = -sumY;
      if(sumY > 255)    sumY = 255;

      SUM = sumX + sumY;
      
      if(SUM > 255)    SUM = 255; 
   
      orow = 255  - (unsigned char)(SUM);
      return ((unsigned char) orow);

}
 
