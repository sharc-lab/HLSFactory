//========================================================================================
// 
// File Name    : vga.h
// Description  : vga implemenetation
// Release Date : 10/02/2015
// Author       : PolyU DARC Lab
//                Siyuan Xu
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version    Author            Description
//---------------------------------------------------------------------------------------
//10/02/2015       1.1     PolyU DARClab     main vga definition header            
//=======================================================================================

#include "vga.h"
void vga::vga_main(void){

	const int  h_pulse=5; //--horiztonal sync pulse width in pixels
	const int h_bp=10; //--horiztonal back porch width in pixels
	const int h_pixels=20; //--horiztonal display width in pixels
	const int h_fp=5; //--horiztonal front porch width in pixels
	bool h_pol=false; //--horizontal sync pulse polarity (1 = positive, 0 = negative)
	const int v_pulse=3; //--vertical sync pulse width in rows
	const int v_bp=5; //--vertical back porch width in rows
	const int v_pixels=10; //--vertical display width in rows
	const int v_fp=1;//--vertical front porch width in rows
	bool v_pol=true; //--vertical sync pulse polarity (1 = positive, 0 = negative)
	const int h_period = h_pulse + h_bp + h_pixels + h_fp; //--total number of pixel clocks in a row
	const int v_period = v_pulse + v_bp + v_pixels + v_fp;  //--total number of rows in column
	int h_count =0; // --horizontal counter (counts the columns)
	int v_count =0;  //  --vertical counter (counts the rows)
       	n_blank.write(1);  //--no direct blanking
 	 n_sync.write(0); //--no sync on green
	wait();

while(1){

      if (h_count < h_period - 1)    //--horizontal counter (pixels)
	{ h_count = h_count + 1; } 
      else  
	 { h_count = 0;
     	 if(v_count < v_period - 1)  //--veritcal counter (rows)
		 {v_count = v_count + 1;}
    	  else
		 { v_count = 0;}
         }

     //horizontal sync signal
      if (h_count < h_pixels + h_fp || h_count > h_pixels + h_fp + h_pulse)
	{ h_sync.write (! h_pol) ;  }  //deassert horiztonal sync pulse
      else 
	{ h_sync.write (h_pol) ;   }   //assert horiztonal sync pulse

      //vertical sync signal
      if (v_count < v_pixels + v_fp || v_count > v_pixels + v_fp + v_pulse) 
	 {v_sync.write (! v_pol) ;  }  //deassert vertical sync pulse
      else
	{v_sync.write (v_pol) ; }      //assert vertical sync pulse
 
     //set pixel coordinates
      if (h_count < h_pixels)			//horiztonal display time 
	  { column.write ( h_count );   }    //set horiztonal pixel coordinate
      if (v_count < v_pixels) 			//vertical display time
	 {  row.write (v_count) ;   }        // set vertical pixel coordinate
   
     //set display enable output
      if (h_count < h_pixels && v_count < v_pixels) //display time
	 {disp_ena.write (true) ;   }                // enable display     
      else  					//blanking time
	{ disp_ena.write (false);  }  		//disable display

wait();

}
}





