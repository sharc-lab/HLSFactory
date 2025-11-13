//========================================================================================
// 
//
// File Name    : main.cpp
// Description  : Top module for IDCT  system
// Release Date : 31/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra  
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version  Author              Description
//----------------------------------------------------------------------------------------
// 31/02/2013      1.0   PolyU  DARC Lab      Main IDCT top module                 
//
// g++ -o idct.exe main.cpp tb_idct.cpp idct.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//=======================================================================================


#include "idct.h"
#include "tb_idct.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;

  sc_signal<  bool >         input_ready;
  sc_signal< sc_int<16>  >   input_coef;
  sc_signal< sc_uint<6>  >   input_quant;

  sc_signal< bool >         output_start;
  sc_signal< sc_uint<8>  >  output_sample;
  sc_signal< sc_uint<3> >   output_row;
  sc_signal< sc_uint<3> >   output_col;


   idct u_IDCT("IDCT");
   test_idct u_test("test_IDCT");

   //connect IDCT to test module
   u_IDCT.clk( clk );
   u_IDCT.rst( rst );
 
  u_IDCT.input_ready( input_ready );
  u_IDCT.input_coef( input_coef );
  u_IDCT.input_quant( input_quant );

  u_IDCT.output_start( output_start );
  u_IDCT.output_sample( output_sample );
  u_IDCT.output_row( output_row );
  u_IDCT.output_col( output_col );
 
  // connect to testbench
   u_test.clk( clk );
   u_test.rst( rst );
 
  u_test.input_ready( input_ready );
  u_test.input_coef( input_coef );
  u_test.input_quant( input_quant );

  u_test.output_start( output_start );
  u_test.output_sample( output_sample );
  u_test.output_row( output_row );
  u_test.output_col( output_col );


#ifdef WAVE_DUMP
  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, clk              , "clk");
  sc_trace(trace_file, rst              , "rst");
 
   sc_trace(trace_file, input_ready     , "input_ready"); 
   sc_trace(trace_file, input_coef      , "input_coef");
   sc_trace(trace_file, input_quant      , "input_quant");

   sc_trace(trace_file, output_start    , "output_start"); 
   sc_trace(trace_file, output_sample   , "output_sample");
   sc_trace(trace_file, output_row      , "output_row");
   sc_trace(trace_file, output_col      , "output_col");
 

#endif  // End WAVE_DUMP


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
