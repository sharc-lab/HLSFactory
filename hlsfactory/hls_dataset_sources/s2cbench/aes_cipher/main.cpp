//===========================================================================
//
//
// File Name    : main.cpp
// Description  : AES CIPHER and INV CHIPER  main
// Release Date : 24/11/2014 
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Shuangnan Liu
//
// Revision History
//---------------------------------------------------------------------------
// Date             Author      Version     Description
//---------------------------------------------------------------------------
//14/02/2013        PolyU           1.0     Main system description
//24/11/2014        PolyU           1.1     cipher and inv cipher combined into same file
//
// g++ -o aes_cipher.exe main.cpp tb_aes_cipher_3G.cpp aes_cipher.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//===========================================================================

#include "aes.h"
#include "tb_aes.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;
 // sc_signal<bool>         mode;
  sc_signal<sc_uint<8> >  in_data[SIZE];
  sc_signal<sc_uint<8> >  in_key[SIZE];
  sc_signal<sc_uint<8> >  out_data[SIZE];


  int x;
  char name[10];


  aes 	 u_AES("AES");
  test_aes test("test_AES");

  //connect to AES
  u_AES.clk( clk );
  u_AES.rst( rst );
 // u_AES.mode( mode );

  for(x=0; x < SIZE; x++){
    u_AES.idata[x]( in_data[x] );
    u_AES.ikey[x]( in_key[x] );
    u_AES.odata[x]( out_data[x] );
  }


  // connect to Test Bench
  test.clk( clk );
  test.rst( rst );
 // test.mode( mode );

  for(x=0; x < SIZE; x++){
    test.idata[x]( in_data[x] );
    test.ikey[x]( in_key[x] );
    test.odata[x]( out_data[x] );
  }


#ifdef WAVE_DUMP
  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, clk             , "clk");
  sc_trace(trace_file, rst             , "rst");
 // sc_trace(trace_file, mode            , "mode");

 for(x=0; x < SIZE; x++){
   sprintf(name, "in_data%d",x);   
   sc_trace(trace_file, in_data[x]          , name);

   sprintf(name, "in_key%d",x);   
   sc_trace(trace_file, in_key[x]            , name);

   sprintf(name, "out_data%d",x);   
   sc_trace(trace_file, out_data[x]            , name);
 
  }

#endif  // End WAVE_DUMP

  sc_start( 25, SC_NS );
  rst.write(0);
  //mode.write(0);  // Encrypt DATA. Set to 1 to decrypt

  
  sc_start( 25, SC_NS );
  rst.write(1);

  
  sc_start(1000, SC_NS); 
  sc_start();
#ifdef WAVE_DUMP
   sc_close_vcd_trace_file(trace_file);
#endif

    return 0;

};





