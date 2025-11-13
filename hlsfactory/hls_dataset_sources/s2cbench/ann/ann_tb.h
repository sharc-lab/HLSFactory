//========================================================================================
// 
// File Name    : ann_tb.h
// Description  : 
// Release Date : 22/10/2014
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 2014          1.0            David Aledo    ANN testbench header
// 2014          1.1            B Carrion Schafer  added 2 and 4 layer configuration
//=====================================================================================


#ifndef ANN_TB_H
#define ANN_TB_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h> //rand
#include <time.h>

//#include "tb_confg.h" // Configuration will be done trough sc_main args
#include "synth_param.h" // synthesizable parameters
#include "config.h"
#include "train_parameters.h"
#include "image_parameters.h"
#include "layer.h"

#include <systemc.h>
#include <sys/types.h>
#include <math.h>

extern int *a[Nlayer-1]; //global variable for training

SC_MODULE(ann_tb)
{

  //Inputs
   sc_in< bool > clk;
   sc_in< bool > rst;

   sc_in< sc_int<NbitOut> > outputs[NumO];
   sc_in< bool > run_out;

   // Outputs
   sc_out< bool > run_in;
   sc_out< sc_int<NbitIn> > inputs[NumI];   
   sc_out< bool > wr;
   sc_out< sc_int<NbitW> > wdata;


   /* Variables */
   FILE *image_in, *im_out;
   unsigned char *image, *result;
   char *bmp_file_in, *bmp_file_out;
   char *bmp_file_in_check, *bmp_file_out_check;

   BITMAPINFOHEADER bitmapInfoHeader;
   BITMAPFILEHEADER bitmapFileHeader; 
   unsigned char biColourPalette[1024];

   int N_tr_block;

   layer *nn;

   /* Functions */

   /* D */
   double derived_saturated(int);

   /* E */
   double error_calculation(int *, double *);

   /* I */
   void image_write(char *);

   /* L */
   unsigned char *load_bitmapfile(const char *im_path);
   void learning_cthread(); 


   /* Constructors */
   SC_HAS_PROCESS(ann_tb);
   ann_tb(sc_module_name nm);

   /* Destructor */
   ~ann_tb();
};

#endif
