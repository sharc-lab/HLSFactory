//---------------------------------------------------------------------------
//
//
// File Name    : snow.h
// Description  : stream cipher that forms theheart of the 3GPP confidentiality
//                algorithm UEA2 and the 3GPP integrity algorithm UIA2
//
// Release Date : 26/07/13
// Author       : ETSI/SAGE (www.gsm.com)
// Modified     : PolyU DARC Lab
//
// Revision History
//---------------------------------------------------------------------------
// Date      Version   Author          Description
//---------------------------------------------------------------------------
// 2006        1.0    ETSI/SAGE        Original ANSI C description
//26/07/13     1.1    PolyU DARC Lab   Converted into Synthesizable SystemC
//---------------------------------------------------------------------------


#ifndef SNOW_3G_H
#define SNOW_3G_H

#include "define.h"


typedef sc_uint<8> u8;
typedef sc_uint<32> u32;
typedef sc_biguint<64> u64;


SC_MODULE (snow_3G) {
 
 // inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<32> >  ks_in[SIZE];

  // outputs
  sc_out<sc_uint<32> >  ks_out[SIZE];


  /* Variables declaration*/
  sc_uint<32> ks[SIZE];

  /* LFSR */
  u32 LFSR_S0 ;
  u32 LFSR_S1 ;
  u32 LFSR_S2 ;
  u32 LFSR_S3 ;
  u32 LFSR_S4 ;
  u32 LFSR_S5 ;
  u32 LFSR_S6 ;
  u32 LFSR_S7 ;
  u32 LFSR_S8 ;
  u32 LFSR_S9 ;
  u32 LFSR_S10 ;
  u32 LFSR_S11 ;
  u32 LFSR_S12 ;
  u32 LFSR_S13 ;
  u32 LFSR_S14 ;
  u32 LFSR_S15 ;
  /* FSM */
  u32 FSM_R1 ;
  u32 FSM_R2 ;
  u32 FSM_R3 ; 


  /* C */
  sc_uint<32> ClockFSM();
  void ClockLFSRInitializationMode(sc_uint<32>);
  void ClockLFSRKeyStreamMode();


  /* D */
  sc_uint<32> DIValpha(sc_uint<8>);

  /* G */
  void GenerateKeystream();
  
  /* I */
  void Initialize(sc_uint<32> *k, sc_uint<32> *);

  /* M */
  sc_uint<8> MULx(sc_uint<8>, sc_uint<8>);

  template <class T>
  sc_uint<8> MULxPOW(sc_uint<8> , T, sc_uint<8>);
  sc_uint<32>  MULalpha(sc_uint<8>);



  /* S */
  sc_uint<32> S1(sc_uint<32>);
  sc_uint<32> S2(sc_uint<32>);



  SC_CTOR (snow_3G){
     SC_CTHREAD(GenerateKeystream,clk.pos());
     reset_signal_is(rst,false);
  };


  ~snow_3G(){};


};

#endif  // SNOW_3G_H
