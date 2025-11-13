//---------------------------------------------------------------------------
//
//
// File Name    : qsort.h
// Description  : Quick sort module declaration
// Release Date : 01/08/13
// Author       : PolyU DARC Group
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------
// Date       Version      Author       Description
//---------------------------------------------------------------------------
//            1.0       Darel Rex Finley   Initial version. This public-domain C implementation by
// 01/08/13   1.1       PolyU DARC Group   Conversion into Synthesizable SystemC
//---------------------------------------------------------------------------

#ifndef QSORT_H
#define QSORT_H

#include "define.h"


SC_MODULE( quicksort )
{
    sc_in<bool> clk;
    sc_in<bool> rst;

   sc_in<sc_uint<8> > indata;
   sc_out<sc_uint<8> > odata;


   /* R */
   void run() ;

   /* S */
   void swap(int *, int *);
   void sort(sc_uint<8> *);

	
   SC_CTOR( quicksort)
     {
       SC_CTHREAD(run, clk.pos()) ;
       reset_signal_is(rst, false) ;
     }

   ~quicksort(){}

};

#endif // QOSRT_H

