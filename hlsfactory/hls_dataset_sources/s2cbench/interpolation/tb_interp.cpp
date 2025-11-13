//========================================================================================
//
// File Name    : tb_interp.cpp
// Description  : Send/receive and compare results parts of Testbench
// Release Date : 23/02/2018
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra, Jianqi Chen
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date           Version     Author    Description
//----------------------------------------------------------------------------------------
// 23/07/2013     1.0         PolyU     TB stimuli generator and data receive and compare
// 23/02/2018     1.1         UTD       increase number of data outputs to 4
//=======================================================================================
#include "tb_interp.h"

//----------------------
// Send data to Interpolation Filter
//----------------------------
void tb_interp::send(void){

	// Variables declarations
	float indata_var;
	int k;


	//Reset initialization
	in_file_data = fopen(IN_DATA, "r");

	if(!in_file_data){
		cout<< endl << IN_DATA << " data file could not be opened " << endl;
		sc_stop();
	}

	wait();

	while(true){

		while(fscanf(in_file_data,"%f",&indata_var) !=EOF ){
			indata.write(indata_var);
			wait(); 
		}
		indata.write(0);
		wait();
		wait();
		wait();
		wait();
		wait();

		fclose(in_file_data);

		cout << endl << "Start comparing results" << endl;
		compare_results();
		sc_stop();

		wait();
	}// end of while loop

}


//---------------------------------------
// Receive data from Interpolation Filter
//--------------------------------------
void tb_interp::recv(void){

	bool odata_en_rcv;
	float  odata_rcv;
	int i;


	fptr_data = fopen(OUT_FILE , "wt");
	fptr_en =   fopen(OUT_FILE_ENABLE, "wt");

	if(!fptr_data){
		cout << endl << OUT_FILE << " could not be opened" << endl;
		sc_stop();
	}

	if(!fptr_en){
		cout << endl << OUT_FILE_ENABLE <<  " could not be opened" << endl;
		sc_stop();
	}

	wait();
	wait();
	wait();
	wait();
	wait();
	wait();
	
	while(true){

	odata_en_rcv = odata_en.read();
	for(i=0;i<4;i++){
		odata_rcv = odata[i].read();
		fprintf(fptr_en,"%c\n",odata_en_rcv?'1':'0');
		fprintf(fptr_data,"%f\n",odata_rcv);
	}
	fprintf(fptr_data,"\n",odata_rcv);
	wait();
	}//end of while


}

//--------------------------------
// Compare simulation results vs. golden output
//--------------------------------

void tb_interp::compare_results(void){

	// Variables declaration
	float outfilter_golden,outfilter,golden_en, outfilter_en;
	int errors1, errors2, line1, line2;
	FILE *out_fil_golden;
	FILE *out_fil_golden_en; 
	FILE *diff_file;


	fclose(fptr_en);
	fclose(fptr_data);

	fptr_en = fopen(OUT_FILE_ENABLE, "rt");
	fptr_data = fopen(OUT_FILE, "rt");
	diff_file = fopen(DIFF_OUT,"rt");

	line1 =0;
	errors1 = 0;
	line2 = 0;
	errors2=0;


	out_fil_golden = fopen(GOLDEN_OUTPUT , "rt");
	out_fil_golden_en = fopen(GOLDEN_OUTPUT_ENABLE, "rt");

	if(!out_fil_golden){
		cout << "Could not open " << GOLDEN_OUTPUT << endl;
		sc_stop();
	}


	if(!out_fil_golden_en){
		cout << "Could not open " << GOLDEN_OUTPUT_ENABLE << endl;
		sc_stop();
	}

	cout << "Verfying Ouptut data " <<  endl; 

	while(fscanf(out_fil_golden, "%f", &outfilter_golden)!= EOF && fscanf(fptr_data, "%f", &outfilter) != EOF ){

	cout << endl << "Cycle["<< line1 << "]:" << outfilter_golden << " -- " << outfilter;

	if(outfilter!= outfilter_golden){
		cout<< "\nOutput mismatch [line:" << line1 << "] Golden:" << outfilter_golden << "--output:"<<outfilter;
		fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %f -- Output: %f",line1, outfilter_golden, outfilter);
		errors1++;
	}

	line1++;

	}

	cout << endl<< endl << "Verfying Ouptut enable signal " <<  endl; 

	while(fscanf(out_fil_golden_en, "%f", &golden_en)!= EOF &&  fscanf(fptr_en, "%f",&outfilter_en) != EOF){

	cout<< endl <<  "Cycle["<< line2 << "]:" << golden_en << "--"<< outfilter_en;

	if(golden_en != outfilter_en){
		cout<<"\n Output enable mismatch [line:" << line1 << "] Golden: " << golden_en << " -- output: "<<outfilter_en;
		errors2 ++;
	}

	line2 ++;
	}


	if(errors1 == 0 && errors2 == 0)
		cout<< endl << endl << "Finished simulation successfully" << endl;
	else
		cout<< endl<< "Finished simulation with "<< errors1 << " and " << errors2<< " mismatches between Golden and simulation"<<endl;

	fclose(out_fil_golden_en);
	fclose(out_fil_golden);
	fclose(fptr_en);
	fclose(fptr_data);
	fclose(diff_file);
}
