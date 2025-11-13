//========================================================================================
// 
// File Name    : vga_top.cpp
// Description  : vga top
// Release Date : 10/02/2015
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Xu Siyuan
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//10/02/2015      1.1       PolyU    TOP VGA main description
//=======================================================================================

#ifndef TOP_
#define TOP_

#include <systemc.h>
#include "vga.cpp"
#include "image.cpp"

SC_MODULE(top_vga)
{
   sc_in_clk sys_clk;   //--pixel clock at frequency of VGA mode being used
   sc_in <bool> reset_n;  //--active low asycnchronous reset
   sc_out< bool >  h_sync ;//--horiztonal sync pulse
   sc_out< bool >  v_sync ;//--vertical sync pulse
   sc_out< bool >   n_blank  ;//--direct blacking output to DAC
   sc_out< bool >   n_sync  ;//--sync-on-green output to DAC
   sc_out <sc_uint<8> > red; //red magnitude output to DAC
   sc_out <sc_uint<8> > green; //green magnitude output to DAC
   sc_out<sc_uint<8> > blue; //blue magnitude output to DAC
	
    vga vga1;	//component
    image image1;  //component


   sc_signal< bool >  disp_ena  ;//--display enable ('1' = display time, '0' = blanking time)
   sc_signal< int >  column  ;//--horizontal pixel coordinate
   sc_signal< int >   row   ;//--vertical pixel coordinate

  

 SC_CTOR (top_vga) :vga1("vga1"),image1("image1"){

    //vga module instantiate
    vga1.sys_clk( sys_clk);   //--pixel clock at frequency of VGA mode being used
    vga1.reset_n(reset_n);  //--active low asycnchronous reset
    vga1.h_sync(h_sync);   //--horiztonal sync pulse
    vga1.v_sync(v_sync) ;//--vertical sync pulse
    vga1.disp_ena(disp_ena)  ;//--display enable ('1' = display time, '0' = blanking time)
    vga1.column  (column );//--horizontal pixel coordinate
    vga1.row  (row ) ;//--vertical pixel coordinate
    vga1.n_blank (n_blank ) ;//--direct blacking output to DAC
    vga1.n_sync (n_sync );//--sync-on-green output to DAC

    //hw_image module instantiate
    image1.sys_clk(sys_clk);
    image1.reset_n(reset_n);
    image1.disp_ena(disp_ena); //display enable ('1' = display time, '0' = blanking time)
    image1.column(column); // row pixel coordinate
    image1. row(row);  //column pixel coordinate
    image1. red(red); //red magnitude output to DAC
    image1. green(green); //green magnitude output to DAC
    image1.blue(blue); //blue magnitude output to DAC

 	  };


};
#endif  //TOP_
