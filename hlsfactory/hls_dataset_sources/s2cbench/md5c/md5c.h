//=============================================================================
//
// File Name    : md5c.h
// Description  : MD5 message-digest algorithm
// Release Date : 16/07/13
// Author       : RSA Data Security, Inc. 
// Modified     : PolyU
//
// Revision History
//---------------------------------------------------------------------------
// Date       Version      Author       Description
//---------------------------------------------------------------------------
// 1991       1.0          RSA Data Security, Inc
//16/07/13    1.1           PolyU        Converted into Synthesizable SystemC
//==========================================================================


#ifndef MD5C_H_
#define MD5C_H_

#include "define.h"


SC_MODULE( md5c ) {

  // Inputs
  sc_in_clk              clk;
  sc_in< bool >          rst;

  sc_in< bool >          i_req;
  sc_in< bool >          i_final;
  sc_in< sc_uint<32> >   i_inputLen;

  // Outputs
  sc_out< bool >         o_busy;
  sc_out< bool >         o_start;
  sc_out< sc_uint<8> >   o_digest;


  // Member variables declaration
  sc_uint<32>		m_inputLen;
  sc_uint<8>		m_input[ MD5C_INPUT_BUFSIZE ];

  sc_uint<32>	m_state[4];	/* state (ABCD) */
  sc_uint<32>	m_count[2];	/* number of bits, modulo 2^64 (lsb first) */
  sc_uint<8>	m_buffer[64];	/* input buffer */


  /* E */
  void entry();


  /* M */
  void MD5Init();
  void MD5Update();
  void MD5Final();
  void MD5Pad();
  void MD5Transform();



  SC_CTOR( md5c ){
      SC_CTHREAD( entry, clk.pos() );
      reset_signal_is(rst,false);
    }

  ~md5c(){};


};

#endif // MD5C_H_

