//========================================================================================
// 
// File Name    : train_parameters.h
// Description  : 
// Release Date : 22/10/2014
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 2014          1.0            David Aledo     Exploration trainig parameters
//
//=======================================================================================
#ifndef TRAIN_PARAM_H
#define TRAIN_PARAM_H

#define N_ITER  4000  // Training iterations
#define GOAL_ERROR 30 // Exit if error is smaller
#define Nu_ERROR  1.03   // Maximun error ratio to not discard actual weights
#define Ro_DECREASE  0.9 // Constant for decreasing Lr when learning is not converging
#define Ephsylon_INCREASE  1.04 // Constant for increasing Lr when the error is improving
#define Lr_START  1e-8  // Initial Lr
#define RAND_RANGE (1024*4) // Range of the initial random weights


#endif
