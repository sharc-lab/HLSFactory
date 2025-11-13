//========================================================================================
// 
// File Name    : main.cpp
// Description  : Main function for the simulation and training of the Artificial Neural Network (ANN)
//                The traing is peformed at the testbench ann_tb.cpp), while the ANN is 
//                described in ann.cpp (this is also the synthesizable part)
// Release Date : 22/10/2014
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author                        Description
//----------------------------------------------------------------------------------------
// 2014          1.0            David Aledo, UPM              Initial version
// 2014          1.1            B. Carrion Schafer, DARClab   Accept different bmp files
//
//=======================================================================================

/* libraries */
#include <systemc.h>

#include <iostream>
#include <stdlib.h>
#include <string.h>

/* Include modules */
#include "ann_tb.h"
#include "ann.h"

/* Parameters */
#include "synth_param.h"

int *a[Nlayer-1]; //outputs of the hidden layers for backpropagation. global variable for training

int sc_main(int argc, char** argv)
{

   char name[10];


   cout << "Starting ANN" << endl;

   for(int l=0; l<(Nlayer-1); l++)
   {
      a[l] = new int[NumN[l]];
   }

   /* Modules */
   ann u_ann("ann");
   ann_tb train_tb("train_tb");

   /* Signals */
   sc_clock clk("clk", 10, SC_NS, 0.5, 5, SC_NS, true);
   sc_signal< bool > rst;
   sc_signal< bool > run_in;
   sc_signal< sc_int<NbitIn> > inputs[NumI];
   sc_signal< bool > wr;
   sc_signal< sc_int<NbitW> > wdata;
   sc_signal< sc_int<NbitOut> > outputs[NumO];
   sc_signal< bool > run_out;

   /* Connections */
   u_ann.clk(clk);
   u_ann.rst(rst);
   u_ann.run_in(run_in);
   for(int i=0; i<NumI; i++)
      u_ann.inputs[i](inputs[i]);
   u_ann.wr(wr);
   u_ann.wdata(wdata);
   u_ann.run_out(run_out);
   for(int i=0; i<NumO; i++)
      u_ann.outputs[i](outputs[i]);

   train_tb.clk(clk);
   train_tb.rst(rst);
   train_tb.run_in(run_in);
   for(int i=0; i<NumI; i++)
      train_tb.inputs[i](inputs[i]);
   train_tb.wr(wr);
   train_tb.wdata(wdata);
   train_tb.run_out(run_out);
   for(int i=0; i<NumO; i++)
      train_tb.outputs[i](outputs[i]);

   if(argc > 1){
     train_tb.bmp_file_in = (char *)malloc(strlen(argv[1]+1));
     strcpy(train_tb.bmp_file_in, argv[1]);

     train_tb.bmp_file_out = (char *)malloc(strlen(argv[1]+5)); // include _out.bmp
     char *tmp = (char *)malloc(strlen(argv[1])+1);
     strncpy(tmp, argv[1], strlen(argv[1]+4));
     sprintf(train_tb.bmp_file_out,"%s_out.bmp", tmp);
     free(tmp);		     
   }
   else{
     train_tb.bmp_file_in = (char *)malloc(strlen(IMAGE_IN)+1);
     strcpy(train_tb.bmp_file_in, IMAGE_IN);
     
     train_tb.bmp_file_out = (char *)malloc(strlen(IMAGE_OUT)+1);
     strcpy(train_tb.bmp_file_out, IMAGE_OUT);

   }
   // users species image to verify the training
   if(argc > 2){
     train_tb.bmp_file_in_check = (char *)malloc(strlen(argv[2]+1));
     strcpy(train_tb.bmp_file_in_check, argv[2]);

     train_tb.bmp_file_out_check = (char *)malloc(strlen(argv[2]+5)); // include _out.bmp
     char *tmp = (char *)malloc(strlen(argv[2])+1);
     strncpy(tmp, argv[2], strlen(argv[2]+4));
     sprintf(train_tb.bmp_file_out_check,"%s_out.bmp", tmp);
     free(tmp);	
   }
   else{
     train_tb.bmp_file_in_check = (char *)malloc(strlen(IMAGE_IN_CHECK)+1);
     strcpy(train_tb.bmp_file_in_check, IMAGE_IN_CHECK);
     
     train_tb.bmp_file_out_check = (char *)malloc(strlen(IMAGE_OUT_CHECK)+1);
     strcpy(train_tb.bmp_file_out_check, IMAGE_OUT_CHECK);
   }


#ifdef WAVE_DUMP
  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, clk              , "clk");
  sc_trace(trace_file, rst              , "rst");
 
  sc_trace(trace_file, run_in             , "run_in");


  for(int i=0; i<NumI; i++){
   sprintf(name, "inputs%d",i);   
   sc_trace(trace_file, inputs[i]          , name);
  }

  for(int i=0; i<NumO; i++){ 
  sprintf(name, "outputs%d",i);   
   sc_trace(trace_file, outputs[i]          , name);
  }

  sc_trace(trace_file, run_out             , "run_out");
  sc_trace(trace_file, wr                  , "wr");
  sc_trace(trace_file, wdata               , "wdata");


#endif  // End WAVE_DUMP


    cout << "BMP IN " <<  train_tb.bmp_file_in <<  endl;
    cout << "BMP OUT " <<  train_tb.bmp_file_out <<  endl;
    cout << "BMP IN test " <<  train_tb.bmp_file_in_check <<  endl;
    cout << "BMP OUT test " <<  train_tb.bmp_file_out_check <<  endl;

   sc_start();

#ifdef WAVE_DUMP
   sc_close_vcd_trace_file(trace_file);
#endif

   // Free ANN memory allocated
   for(int l=1; l<(Nlayer-1); l++)
   {
      delete [] a[l];
   }

   return 0;
}
