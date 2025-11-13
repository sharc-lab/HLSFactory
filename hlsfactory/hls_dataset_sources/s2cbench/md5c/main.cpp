//========================================================================================
// 
//
// File Name    : main.cpp
// Description  : Top MD5C system
// Release Date : 30/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------------------
// Date            Version    Author         Description
//----------------------------------------------------------------------------------------
// 30/07/2013         1.0    PolyU  DARC Lab    MainMD5C system description               
//
// g++ -o md5c.exe main.cpp tb_md5c.cpp md5c.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//=======================================================================================


#include "md5c.h"
#include "tb_md5c.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;

  sc_signal< bool >         o_busy;
  sc_signal< bool >         o_start;
  sc_signal< sc_uint<8> >   o_digest;    

  sc_signal< bool >          i_req;
  sc_signal< bool >          i_final;
  sc_signal< sc_uint<32> >   i_inputLen;



   md5c u_MD5C("MD5C");
   test_md5c test("test_MD5C");

   //connect to bubble sort
   u_MD5C.clk( clk );
   u_MD5C.rst( rst );

   u_MD5C.o_busy( o_busy );
   u_MD5C.o_start( o_start );   
   u_MD5C.o_digest( o_digest );

   u_MD5C.i_req( i_req );
   u_MD5C.i_final( i_final );
   u_MD5C.i_inputLen( i_inputLen );

 
  // connect to test bench
  test.clk( clk );
  test.rst( rst );
 
   test.o_busy( o_busy );
   test.o_start( o_start );   
   test.o_digest( o_digest );

   test.i_req( i_req );
   test.i_final( i_final );
   test.i_inputLen( i_inputLen );



#ifdef WAVE_DUMP
  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, clk              , "clk");
  sc_trace(trace_file, rst              , "rst");
 
   sc_trace(trace_file, o_busy          , "o_busy"); 
   sc_trace(trace_file, o_start         , "o_start");
   sc_trace(trace_file, o_digest       , "o_digest"); 

   sc_trace(trace_file, i_req         , "i_req"); 
   sc_trace(trace_file, i_final        , "i_final");
   sc_trace(trace_file, i_inputLen       , "i_inputLen"); 

 

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
