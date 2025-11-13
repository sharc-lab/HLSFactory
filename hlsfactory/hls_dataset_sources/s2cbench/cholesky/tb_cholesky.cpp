//========================================================================================
// 
// File Name    : tb_cholespy.cpp
// Description  : Testbench
// Release Date : 12/04/2017
// Author       : UTD DARClab
//                Benjamin Carrion Schafer 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version   Author     Description
//----------------------------------------------------------------------------------------
//14/04/2017       1.0    UTD (BCS)   Cholesky decomposition  testbench
//
//=======================================================================================
#include "tb_cholesky.h"


//--------------------------
// Send data thread
//--------------------------
void test_CHOLESKY::send(){

  // Variables declaration
  int i=0;
  float  in_data_read;

  //Reset routine
  in_file = fopen(INFILENAME, "rt");
  

  if(!in_file){
    cout << "Could not open " << INFILENAME << "\n";
    sc_stop();
    exit (-1);
  }


  for(i=0; i < MATRIX_SIZE; i ++){
    in_data[i].write(0);
  }
  


  wait();
  i= 0;
  while(true){

    while(fscanf(in_file,"%f", &in_data_read) != EOF){

	in_data[i++].write(in_data_read);     
	if(i == MATRIX_SIZE){
	  wait();
	  i= 0;
	}
    }
  
  
    fclose(in_file);

    cout << endl << "Starting comparing results " << endl;
 
    compare_results();
    sc_stop();

    wait();

  }//while_loop
}



//-----------------------------
// Receive data thread
//----------------------------
void test_CHOLESKY::recv(){

  // Variables declaration
  int x=0; 
  float out_write=0;

  out_file = fopen (OUTFILENAME, "wt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit (-1);
  }

  wait();

  while(true){

    for(x=0; x < MATRIX_SIZE; x++){	
    	out_write = (float) cholesky_output[x].read();
	fprintf(out_file,"%0.1f\n",out_write);
	}

    wait();
  }
}


//---------------------------------
// Compare results function
//--------------------------------
void test_CHOLESKY::compare_results(){

  float outdata, outdata_golden;
  int line=1, errors=0;

  // Close file where outputs are stored
  fclose(out_file);

  // Open results file
  out_file = fopen (OUTFILENAME, "rt");

  if(!out_file){
    cout << "Could not open " << OUTFILENAME << endl;
    sc_stop();
    exit (-1);
  }

    //
    // Load the golden output from file
    //
    out_golden_file = fopen (OUTFILENAME_GOLDEN, "rt");
    if(!out_golden_file){
      cout << "Could not open " << OUTFILENAME_GOLDEN << endl;
      sc_stop();
      exit (-1);
     }

    //
    // comparison result with golden output
    //

    diff_file = fopen (DIFFFILENAME, "w");
    if(!diff_file){
	 cout << "Could not open " << DIFFFILENAME<< "\n";
	 sc_stop();
	 exit (-1);
       }


    while(fscanf(out_golden_file, "%f", &outdata_golden) != EOF){
      fscanf(out_file,"%f", &outdata);
     

      cout << endl <<"Cycle["<< line << "]: " << outdata_golden << " -- "<< outdata;

	   if(outdata != outdata_golden){
	     cout << "\nOutput missmatch [line:" << line << "] Golden:" << outdata_golden << " -- Output:" << outdata;
	     fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %f -- Output: %f",line, outdata_golden, outdata);
	     errors++;
	   }

          line ++;
    }

    if(errors == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errors << " MISSMATCHES between Golden and Simulation" << endl;


    fclose(out_file);
    fclose(diff_file);
    fclose(out_golden_file);

}

