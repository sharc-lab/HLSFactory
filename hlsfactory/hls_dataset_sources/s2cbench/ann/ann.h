//========================================================================================
// 
// File Name    : ann.h
// Description  : 
// Release Date : 22/10/2014
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 2014          1.0            David Aledo    ANN header
//
//=====================================================================================

#ifndef ANN_H
#define ANN_H

/* Parameters */
#include "synth_param.h"
#include "config.h"

#include <systemc.h>

#ifdef TRAIN
extern int *a[Nlayer-1]; //global variable for training
#endif

SC_MODULE(ann)
{
  // Inputs
   sc_in< bool > clk;
   sc_in< bool > rst;

   sc_in< bool > run_in;
   sc_in< sc_int<NbitIn> > inputs[NumI];

   sc_in< bool > wr;
   sc_in< sc_int<NbitW> > wdata;

   // Outputs
   sc_out< sc_int<NbitOut> > outputs[NumO];
   sc_out< bool > run_out; 


   /* Internal variables */
   int buff[Nlayer+1][MAX_N] /* ATTR1 */; 

   /* Weight and bias declaration */

#ifdef NLAYER_4
   type_0 layer_0;
   type_1 layer_1;
   type_2 layer_2;
   type_3 layer_3;
#else
   type_0 layer_0;
   type_1 layer_1;
#endif


   /* Processes */
   /* A */
   void ann_cthread();


   /* L */
   template <class Lt>
   void layer(Lt &Wyb, int l);

   /* W */
   void Wyb_cthread();
   template <class Lt>
   void write_Wyb(Lt &Wyb, int l);


   /* Constructors */
   SC_HAS_PROCESS(ann);
   ann(sc_module_name nm);

   /* Destructor */
   //~ann();
};

#endif //ANN_H

