//========================================================================================
// 
// File Name    : tb_top.cpp
// Description  : Testbench
// Release Date : 10/02/2015
// Author       : PolyU DARClab
//               Siyuan Xu
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version   Author     Description
//----------------------------------------------------------------------------------------
//10/02/2015       1.1     PolyU DARClab     top vga  testbench
//
//=======================================================================================
#include "tb_vga.h"


//-----------------------------
// Receive data thread
//----------------------------
void test_top::recv(){

  // Variables declaration
	unsigned int n=0;
   unsigned int h_sync_out_write=0;
   unsigned int v_sync_out_write=0;
   unsigned int  n_blank_out_write=0;
  unsigned int n_sync_out_write=0;
  unsigned int red_out_write=0;
  unsigned int blue_out_write=0;
  unsigned int green_out_write=0;

  out_h_sync_file = fopen (OUTH_SYNC, "wt");
  out_v_sync_file = fopen (OUTV_SYNC, "wt");
  out_n_blank_file = fopen (OUTN_BLANK, "wt");
  out_n_sync_file = fopen (OUTN_SYNC, "wt");
  out_red_file = fopen (OUTRED, "wt");
  out_blue_file = fopen (OUTBLUE, "wt");
  out_green_file = fopen (OUTGREEN, "wt");



  wait();

  while(1){


while(1){
    h_sync_out_write = h_sync.read();
    v_sync_out_write = v_sync.read();
    n_blank_out_write = n_blank.read();
    n_sync_out_write = n_sync.read();
    red_out_write = red.read();
    blue_out_write = blue.read();
    green_out_write = green.read();


    fprintf(out_h_sync_file,"%d\n",h_sync_out_write);
    fprintf(out_v_sync_file,"%d\n",v_sync_out_write);
    fprintf(out_n_blank_file,"%d\n",n_blank_out_write);
    fprintf(out_n_sync_file,"%d\n",n_sync_out_write);
    fprintf(out_red_file,"%d\n",red_out_write);
    fprintf(out_blue_file,"%d\n",blue_out_write);
    fprintf(out_green_file,"%d\n",green_out_write);
     n++;
	if(n==1000) break;
	wait();

}
cout << endl << "Starting comparing results " << endl;
 
    compare_results();
    sc_stop();

    wait();

  }
}

//---------------------------------
// Compare results function
//--------------------------------
void test_top::compare_results(){

  int outred, red_golden, linered=1, errorsred=0;
  int outblue, blue_golden, lineblue=1, errorsblue=0;
  int outgreen, green_golden, linegreen=1, errorsgreen=0;

// Close file where outputs are stored
   fclose(out_h_sync_file);
   fclose(out_v_sync_file);
   fclose(out_n_blank_file);
   fclose(out_n_sync_file);
   fclose(out_red_file);
   fclose(out_blue_file);
   fclose(out_green_file);

  // Open results file
  out_red_file = fopen (OUTRED, "rt");

  if(!out_red_file){
    cout << "Could not open " << OUTRED << endl;
    sc_stop();
    exit (-1);
  }

    //
    //Load the golden output from file
    //
    
    red_golden_file = fopen (RED_GOLDEN, "rt");
    if(!red_golden_file){
      cout << "Could not open " << RED_GOLDEN << endl;
      sc_stop();
      exit (-1);
     }

    //
    // comparison result with golden output
    //

    diffred_file = fopen (DIFFREDFILENAME, "w");
    if(!diffred_file){
	 cout << "Could not open " << DIFFREDFILENAME<< "\n";
	 sc_stop();
	 exit (-1);
       }

   
    while(fscanf(red_golden_file, "%d", &red_golden) != EOF){
      fscanf(out_red_file,"%d", &outred);
     

      cout << endl <<"Cycle["<< linered << "]: " << red_golden << "-- "<< outred;


	   if(outred != red_golden){
	     cout << "\nOutput missmatch [line:" << linered << "] Golden:" << red_golden << " -- Output:" << outred;

	     fprintf(diffred_file,"\nOutput missmatch[line:%d] Golden: %d -- Output: %d",linered, red_golden, outred);
	     
	     errorsred++;
	   }

          linered ++;

    }

    if(errorsred == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errorsred << " MISSMATCHES between Golden and Simulation" << endl;


    fclose(out_red_file);
    fclose(diffred_file);
    fclose(red_golden_file);

 // Open results file
  out_blue_file = fopen (OUTBLUE, "rt");

  if(!out_blue_file){
    cout << "Could not open " << OUTBLUE << endl;
    sc_stop();
    exit (-1);
  }

    //
    //Load the golden output from file
    //
    
    blue_golden_file = fopen (BLUE_GOLDEN, "rt");
    if(!blue_golden_file){
      cout << "Could not open " << BLUE_GOLDEN << endl;
      sc_stop();
      exit (-1);
     }

    //
    // comparison result with golden output
    //

    diffblue_file = fopen (DIFFBLUEFILENAME, "w");
    if(!diffblue_file){
	 cout << "Could not open " << DIFFBLUEFILENAME<< "\n";
	 sc_stop();
	 exit (-1);
       }

   
    while(fscanf(blue_golden_file, "%d", &blue_golden) != EOF){
      fscanf(out_blue_file,"%d", &outblue);
     

      cout << endl <<"Cycle["<< lineblue << "]: " << blue_golden << "-- "<< outblue;


	   if(outblue != blue_golden){
	     cout << "\nOutput missmatch [line:" << lineblue << "] Golden:" << blue_golden << " -- Output:" << outblue;

	     fprintf(diffblue_file,"\nOutput missmatch[line:%d] Golden: %d -- Output: %d",lineblue, blue_golden, outblue);
	     
	     errorsblue++;
	   }

          lineblue ++;

    }

    if(errorsblue == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errorsblue << " MISSMATCHES between Golden and Simulation" << endl;


    fclose(out_blue_file);
    fclose(diffblue_file);
    fclose(blue_golden_file);

 // Open results file
  out_green_file = fopen (OUTGREEN, "rt");

  if(!out_green_file){
    cout << "Could not open " << OUTGREEN << endl;
    sc_stop();
    exit (-1);
  }

    //
    //Load the golden output from file
    //
    
    green_golden_file = fopen (GREEN_GOLDEN, "rt");
    if(!green_golden_file){
      cout << "Could not open " << GREEN_GOLDEN << endl;
      sc_stop();
      exit (-1);
     }

    //
    // comparison result with golden output
    //

    diffgreen_file = fopen (DIFFGREENFILENAME, "w");
    if(!diffgreen_file){
	 cout << "Could not open " << DIFFGREENFILENAME<< "\n";
	 sc_stop();
	 exit (-1);
       }

   
    while(fscanf(green_golden_file, "%d", &green_golden) != EOF){
      fscanf(out_green_file,"%d", &outgreen);
     

      cout << endl <<"Cycle["<< linegreen << "]: " << green_golden << "-- "<< outgreen;


	   if(outgreen != green_golden){
	     cout << "\nOutput missmatch [line:" << linegreen << "] Golden:" << green_golden << " -- Output:" << outgreen;

	     fprintf(diffgreen_file,"\nOutput missmatch[line:%d] Golden: %d -- Output: %d",linegreen, green_golden, outgreen);
	     
	     errorsgreen++;
	   }

          linegreen ++;

    }

    if(errorsgreen == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errorsgreen << " MISSMATCHES between Golden and Simulation" << endl;


    fclose(out_green_file);
    fclose(diffgreen_file);
    fclose(green_golden_file);

 

}



