//========================================================================================
// 
// File Name    : vga.h
// Description  : vga controller declaration
// Release Date : 10/02/2015
// Author       : PolyU DARClab
//                Siyuan Xu
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version   Author              Description
//---------------------------------------------------------------------------------------
//10/02/2015      1.1     PolyU DARClab      main component declaration of vga controller
//============================================================================
#ifndef VGA_H_
#define VGA_H_

#include <systemc.h>

SC_MODULE(vga)
{
   sc_in_clk sys_clk;   //--pixel clock at frequency of VGA mode being used
   sc_in<bool> reset_n;  //--active low asycnchronous reset
   sc_out< bool >  h_sync ;//--horiztonal sync pulse
   sc_out< bool >  v_sync ;//--vertical sync pulse
   sc_out< bool >  disp_ena  ;//--display enable ('1' = display time, '0' = blanking time)
   sc_out< int >  column  ;//--horizontal pixel coordinate
   sc_out< int >   row   ;//--vertical pixel coordinate
    sc_out< bool >   n_blank  ;//--direct blacking output to DAC
   sc_out< bool >   n_sync  ;//--sync-on-green output to DAC
	
    void vga_main(void);

 SC_CTOR (vga) {
 	SC_CTHREAD ( vga_main, sys_clk.pos() );
     	  reset_signal_is(reset_n, false);
	  sensitive <<sys_clk  ;
 	  };
 ~vga(){};

};
#endif  //VGA_H_
