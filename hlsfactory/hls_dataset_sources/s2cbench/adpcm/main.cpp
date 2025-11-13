//========================================================================================
// 
//
// File Name    : main.cpp
// Description  : Top module for ADPCM encoder system
// Release Date : 31/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author              Description
//----------------------------------------------------------------------------------------
// 31/07/2013       1.0    PolyU DARC Lab    ADPCM encoder top system description             
//
// g++ -o adpcm.exe main.cpp tb_adpcm_encoder.cpp adpcm_encoder.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//=======================================================================================


#include "adpcm_encoder.h"
#include "tb_adpcm_encoder.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;

  sc_signal<sc_uint<16> > idata;
  sc_signal<sc_uint<4> > odata;
     

   adpcm u_ADPCM("ADPCM");
   test_adpcm u_test("test_ADPCM");

   //connect ADPCM to test module
   u_ADPCM.clk( clk );
   u_ADPCM.rst( rst );
   u_ADPCM.idata( idata );

   u_ADPCM.odata( odata );
 
  // connect to test bench
  u_test.clk( clk );
  u_test.rst( rst );
  u_test.idata( idata );

  u_test.odata( odata );


#ifdef WAVE_DUMP
  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, clk              , "clk");
  sc_trace(trace_file, rst              , "rst");
 
   sc_trace(trace_file, idata          , "idata"); 
   sc_trace(trace_file,  odata         , "odata");


 

#endif  // End WAVE_DUMP

  sc_start( 10, SC_NS );
  rst.write(1);

  sc_start( 25, SC_NS );
  rst.write(0);

  sc_start( 25, SC_NS );
  rst.write(1);

  sc_start();

#ifdef WAVE_DUMP
   sc_close_vcd_trace_file(trace_file);
#endif

    return 0;

};
