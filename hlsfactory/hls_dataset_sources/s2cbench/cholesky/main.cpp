//===========================================================================
//
// File Name    : main.cpp
// Description  : Cholesky decomposition top system description
// Release Date : 12/04/2017
// Author       : UTD DARClab
//                Benjamin Carrion Schafer 
//
// Revision History
//---------------------------------------------------------------------------
// Date            Version   Author    Description
//---------------------------------------------------------------------------
//14/02/2013         1.0      UTD (BCS)     Top system declaration
//
// g++ -o cholesky.exe cholesky.cpp tb_cholesky.cpp cholesky.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//===========================================================================
#define SC_INCLUDE_FX

#include "cholesky.h"
#include "tb_cholesky.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;
  sc_signal<sc_fixed<16,8,SC_RND,SC_SAT> > in_data[MATRIX_SIZE];
  sc_signal<sc_fixed<16,8,SC_RND,SC_SAT> > out_data[MATRIX_SIZE];
 
  int x;
  char name[10];


  cholesky u_CHOLESKY("CHOLESKY");
  test_CHOLESKY u_test("test_CHOLESKY");

  //connect CHOLESKY
  u_CHOLESKY.clk( clk );
  u_CHOLESKY.rst( rst );

  for(x=0; x < MATRIX_SIZE; x++){
    u_CHOLESKY.in_data[x](in_data[x]);
    u_CHOLESKY.cholesky_output[x](out_data[x]);
  }


  // connect to test bench
  u_test.clk( clk );
  u_test.rst( rst );
  
  for(x=0; x < MATRIX_SIZE; x++){
    u_test.in_data[x](in_data[x]);
    u_test.cholesky_output[x](out_data[x]);	 
 }




#ifdef WAVE_DUMP
  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, clk              , "clk");
  sc_trace(trace_file, rst              , "rst");
 
 for(x=0; x < MATRIX_SIZE; x++){
   sprintf(name, "in_data%d",x);   
   sc_trace(trace_file, in_data[x]          , name);

   sprintf(name, "out_data%d",x);   
   sc_trace(trace_file, out_data[x]          , name);
  }



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
