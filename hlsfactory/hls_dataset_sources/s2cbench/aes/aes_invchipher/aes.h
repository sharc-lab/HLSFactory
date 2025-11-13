//============================================================================
//
//
// File Name    : aes.h
// Description  : ADVANCED ENCRYPTION STANDARD (AES) 128-bit
// Release Date : 16/07/13
// Author       : AES Versoin 1.4 pjc.co.jp. AES Versoin 1.4 pjc.co.jp
// Modified     : PolyU
//
// Revision History
//---------------------------------------------------------------------------
// Date     Author      Version     Description
//---------------------------------------------------------------------------
// 2009    PJC.CO.JP    1.0         Original ANSI C description
//16/07/13  PolyU       1.1         Converted into Synthesizable SystemC
//===========================================================================
#ifndef AES_H
#define AES_H

#include "define.h"

 

SC_MODULE (aes) {
  // input
  sc_in<bool>          clk;
  sc_in<bool>          rst;
  //sc_in<bool>          mode;

  sc_in<sc_uint<8> >   idata[SIZE];
  sc_in<sc_uint<8> >   ikey[SIZE];
  
  sc_out<sc_uint<8> >  odata[SIZE];


  int w[60];                         
  int data[NB];

  sc_uint<8> keys[SIZE];
  sc_uint<8> init[SIZE];


  /* A */
  void AddRoundKey(int *,int);     

  /* C */
  int Computing(int *, int*);  

  /* D */
  int dataget(int *,int);

  /* K */
  void KeyExpansion(sc_uint<8> *, const  int*);     

  /* M */
  void MixColumns(int *);
  int mul(int,int);

  /* S */
  void ShiftRows(int *);   
  void SubBytes(int *, const int*);         
  int SubWord(int, const int*);     
        
  
  /* R */
  int RotWord(int);                
  void run();

  SC_CTOR (aes){
     SC_CTHREAD(run,clk.pos());
     reset_signal_is(rst,false);
  }

  ~aes(){};


};


#endif // AES_H

