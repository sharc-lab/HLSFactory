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

SC_MODULE (aes)
{
    sc_in_clk clk;
    sc_in<bool> rst;

    sc_in<sc_uint<8> > idata[SIZE];
    sc_in<sc_uint<8> > ikey[SIZE];

    sc_out<sc_uint<8> > odata[SIZE];

    void AddRoundKey(U8 state[SIZE], U8 key[SIZE]);
    void SubBytes(U8 state[SIZE]);
    void ShiftRows(U8 state[SIZE]);
    void MixColumns(U8 state[SIZE]);
    void KeyExpansion(U8 current_key[SIZE], U8 rcon, U8 next_key[SIZE]);
    void assign_key(U8 key_in[SIZE], U8 key_out[SIZE]);
    U8 xtime_2(U8 data);
    U8 xtime_3(U8 data);
    U8 xadd(U8 in1, U8 in2);
    int clip(int n);

    void run();

    SC_CTOR(aes)
    {
        SC_CTHREAD(run, clk.pos());
        reset_signal_is(rst, false);
    }

    ~aes() {}
};

#endif // AES_H

