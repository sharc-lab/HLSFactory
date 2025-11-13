//========================================================================================
// 
// File Name    : layer.h
// Description  : 
// Release Date : 22/10/2014
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 2014          1.0            David Aledo     Neuronal Network parameters
//
//=======================================================================================


#ifndef LAYER_H
#define LAYER_H

class layer
{


   /* Dynamic memory */
private:

   bool initialized;

public:
   unsigned int NumIn;
   unsigned int NumN;

   int **W;     // Weights
   float **DW;  // Weight increment for the next epoch
   int **Wprev; // Previous weights
   int *b;      // Bias
   float *Db;   // Bias increment for the next epoch
   int *bprev;  // Previous bias
   int *a;      // Output of each layer
   float *s;    // Sensibilities

   /* Constructors */
   layer();   //Without initialization
   layer(unsigned int n_in, unsigned int n_n); //With initialization

   /* Methods */
   int initialize(unsigned int n_in, unsigned int n_n);
   inline bool is_initialized() {return initialized;}
   /*inline*/ unsigned int get_NumIn() {return NumIn;}
   /*inline*/ unsigned int get_NumN() {return NumN;}

   /* Destructor */
   ~layer();
};

#endif
