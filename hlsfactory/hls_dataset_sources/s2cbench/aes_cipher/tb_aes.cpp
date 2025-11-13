//========================================================================================
// 
//
// File Name    : tb_aes_cipher.cpp
// Description  : Testbench
// Release Date : 29/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Shuangnan Liu 
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version  Author     Description
//----------------------------------------------------------------------------------------
//14/02/2013      1.0    PolyU      aes original testbench
//
//=======================================================================================
#include "tb_aes.h"


/*
** Send data thread
*/
void test_aes::send(){

	// Variables declaration
	int i=0, ret=0;
	unsigned int  in_read, in_read_key;

	//Reset routine
	in_file = fopen(INFILENAME, "rt");

	if(!in_file){
		cout << "Could not open " << INFILENAME << "\n";
		sc_stop();
		exit (-1);
	}

	in_file_key = fopen(INFILENAME_KEY, "rt");

	if(!in_file_key){
		cout << "Could not open " << INFILENAME_KEY << "\n";
		sc_stop();
		exit (-1);
	}

	for(i=0; i < SIZE; i ++){
		idata[i].write(0); 
		fscanf(in_file_key, "%x", &in_read_key);
		input_key[i] = in_read_key;
	}
	i = 0;

	wait();

	while(true){
		while(fscanf(in_file,"%x", &in_read) != EOF){
			idata[(i%4)*4+i/4].write(in_read); //change order(index matrix transverse)
			ikey[(i%4)*4+i/4].write(input_key[i]);
			i++;
	
			if(i == SIZE){
			i = 0;
			wait();
		}
	}

	fclose(in_file);
	fclose(in_file_key);

	cout << endl << "Starting comparing results " << endl;
	
	wait();
	compare_results(); 
	exit(-1);

	}//while_loop
}



/*
** Receive data thread
*/
void test_aes::recv(){

	// Variables declaration
	sc_uint<8> out_write[SIZE];

	int i=0;

	out_file = fopen (OUTFILENAME, "wt");

	if(!out_file){
		cout << "Could not open " << OUTFILENAME << "\n";

		sc_stop();
		exit(-1);
	}


	wait();
	wait();
	while(true){
		wait();
	
		for(i=0; i< SIZE; i++){
			out_write[i] = odata[(i%4)*4+i/4].read(); //change order(index matrix transverse)
			fprintf(out_file,"%x ",(unsigned int)out_write[i]);
		}
		fprintf(out_file,"\n");    
	}

//  fclose(out_file);
}


/*
** Compare results function
*/
void test_aes::compare_results(){

  unsigned int outaes, out_golden, line=1, element=1, errors=0;

  // Close file where outputs are stored
  fclose(out_file);

  //  out_filter_file_read.open(OUTFILENAME);
  //if (mode == 0) {
  out_file = fopen (OUTFILENAME, "rt");
 // }
 // else {
 // out_file = fopen (OUTFILENAME_D, "rt");
 //}

  if(!out_file){
	//if (mode == 0) 
    cout << "Could not open " << OUTFILENAME << "\n";
	//else
	//cout << "Could not open " << OUTFILENAME_D << "\n";
    sc_stop();
    exit(-1);
  }

    //
    //Load the golden pattern
    //

  //if (mode == 0) {
  out_golden_file = fopen (OUTFILENAME_GOLDEN, "rt");
  //}
 // else {
  //out_golden_file = fopen (OUTFILENAME_GOLDEN_D, "rt");
  //}
  if(!out_golden_file){
	//if (mode == 0)
    cout << "Could not open " << OUTFILENAME_GOLDEN << "\n";
	//else
	//cout << "Could not open " << OUTFILENAME_GOLDEN_D << "\n";
    sc_stop();
  exit(-1);
  }

    //
    //Dump the comparison result
    //
    diff_file = fopen (DIFFFILENAME, "w");
    if(!diff_file){
	 cout << "Could not open " << DIFFFILENAME<< "\n";
	 sc_stop();
       }

    while(fscanf(out_golden_file, "%x", &out_golden) != EOF){
      fscanf(out_file,"%x", &outaes);
     
      cout << endl <<"Cycle["<< line << "][" << element << "]: " << out_golden << " -- "<< outaes;

      if(outaes != out_golden){
	cout << "\nOutput missmatch [line:" << line << "] Golden:" << out_golden << " -- Output:" << outaes;
	fprintf(diff_file,"\nOutput missmatch[line:%d][%d] Golden: %u -- Output: %u",line,element, out_golden, outaes);
	  errors++;
      }

      if(element == SIZE){
	element =0;
	line ++;
      }
      element ++;

   }

    if(errors == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errors << " MISSMATCHES between Golden and Simulation" << endl;


    fclose(out_file);
    fclose(diff_file);
    fclose(out_golden_file);


}

