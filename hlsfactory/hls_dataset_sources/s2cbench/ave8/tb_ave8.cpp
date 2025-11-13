//========================================================================================
// 
// File Name    : tb_ave8.cpp
// Description  : Testbench
// Release Date : 12/08/2017
// Author       : DARClab
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version   Author     Description
//----------------------------------------------------------------------------------------
//12/08/2017       1.0    DARClab     ave8  testbench
//
//=======================================================================================
#include "tb_ave8.h"


//--------------------------
// Send data thread
//--------------------------
void test_AVE8::send(){

  // Variables declaration
  int i=0;
  unsigned int  in_ave8_read;

  //Reset routine
  in_ave8_file = fopen(INFILENAME, "rt");


  if(!in_ave8_file){
    cout << "Could not open " << INFILENAME << "\n";
    sc_stop();
    exit (-1);
  }


  wait();
  i= 0;
  while(true){

    while(fscanf(in_ave8_file,"%u", &in_ave8_read) != EOF){
	in_data.write(in_ave8_read);
	  wait();
    }
  
   
    fclose(in_ave8_file);

    cout << endl << "Starting comparing results " << endl;
 
    compare_results();
    sc_stop();

    wait();

  }//while_loop
}



//-----------------------------
// Receive data thread
//----------------------------
void test_AVE8::recv(){

  // Variables declaration
  unsigned int ave8_out_write=0;

  out_ave8_file = fopen (OUTFILENAME, "wt");

  if(!out_ave8_file){
    cout << "Could not open " << OUTFILENAME << "\n";
    sc_stop();
    exit (-1);
  }


  wait();

  while(true){
    ave8_out_write = ave8_output.read();

    fprintf(out_ave8_file,"%d\n",ave8_out_write);

    wait();
  }
}


//---------------------------------
// Compare results function
//--------------------------------
void test_AVE8::compare_results(){

  int outave8, outave8_golden, line=1, errors=0;

  // Close file where outputs are stored
  fclose(out_ave8_file);

  // Open results file
  out_ave8_file = fopen (OUTFILENAME, "rt");

  if(!out_ave8_file){
    cout << "Could not open " << OUTFILENAME << endl;
    sc_stop();
    exit (-1);
  }

    //
    //Load the golden output from file
    //
    out_ave8_golden_file = fopen (OUTFILENAME_GOLDEN, "rt");
    if(!out_ave8_golden_file){
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

    while(fscanf(out_ave8_golden_file, "%d", &outave8_golden) != EOF){
      fscanf(out_ave8_file,"%d", &outave8);
     

      cout << endl <<"Cycle["<< line << "]: " << outave8_golden << "-- "<< outave8;


	   if(outave8 != outave8_golden){
	     cout << "\nOutput missmatch [line:" << line << "] Golden:" << outave8_golden << " -- Output:" << outave8;

	     fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %d -- Output: %d",line, outave8_golden, outave8);
	     
	     errors++;
	   }

          line ++;

    }

    if(errors == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errors << " MISSMATCHES between Golden and Simulation" << endl;


    fclose(out_ave8_file);
    fclose(diff_file);
    fclose(out_ave8_golden_file);



}

