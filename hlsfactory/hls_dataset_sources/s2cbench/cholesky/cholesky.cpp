//========================================================================================
// 
// File Name    : cholesky.cpp
// Description  : cholesky decomposition implementaiton function
// Release Date : 12/04/2017
// Author       : UTD DARClab
//                Benjamin Carrion Schafer 
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//12/04/2017      1.0      UTD (BCS)   Cholesky decomposition main description
//=======================================================================================

#include "cholesky.h"
#define MAX 255 

 const sc_fixed<16,8,SC_RND,SC_SAT> sqrt_lut[128] = { 
	#include "square_root_lut.dat"
};



//  Main thread  
void cholesky::cholesky_main ( void ) {

   // Variables declaration
    sc_fixed<16,8,SC_RND,SC_SAT> in_data_read[MATRIX_SIZE];
    int i;


    // Reset state - should be executable in 1 clock cycle
     wait();  
 

   // Main thread	
   while (1) {

   	 for(i=0;i<MATRIX_SIZE;i++){
	   in_data_read[i] = in_data[i].read();
   	}

	 // Cholesky main computation  function
   	 cholesky_compute(in_data_read);


   	 for(i=0;i<MATRIX_SIZE;i++){
	 	cholesky_output[i].write(L[i]);
   	}

	wait();

}

}

// Cholesky function
void cholesky::cholesky_compute(
    sc_fixed<16,8,SC_RND,SC_SAT>  m1[MATRIX_SIZE])
{
   unsigned int  i,j, k, n=MATRIX_DIM, index_sqrt=0;   
  sc_fixed<16,8,SC_TRN,SC_SAT> s=0;



    for (i = 0; i < n; i++)
        for (j = 0; j < (i+1); j++) {
           s = 0;
            for (k = 0; k < j; k++)
                s += L[i * n + k] * L[j * n + k];

  		if(i == j){
			index_sqrt = m1[i * n + i] - s;
			 L[(i * n) + j] = sqrt_lut[index_sqrt];
	    	}	
		else{

		if( L[j * n + j] != 0)
	         	 L[i * n + j] = (1.0 / L[(j * n) + j] * (m1[(i * n) + j] - s));
		else 
		     L[i * n + j] = 0;	

	}
	 }
}


