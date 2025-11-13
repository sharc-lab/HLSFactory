
//========================================================================================
// 
// File Name    : filter_interp.cpp
// Description  : Main interpolation filter function
// Release Date : 23/02/2018 
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra, Jianqi Chen
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version   Author      Description
//----------------------------------------------------------------------------------------
//23/07/2013     1.0      PolyU      Interpolation filter description
//23/02/2018     1.1      UTD        Changed coefficients
//=======================================================================================
#include "filter_interp.h"


void interp::run ( void ) {


	unsigned int n;
				
	const sc_fixed<17,2,SC_TRN,SC_WRAP> coeff1[TAPS] ={-0.002109243232750,
						       0.017755702728024,
						       -0.077066788574370,
						       0.287601741388967,
						       0.862310282276647,
						       -0.107706831291022,
						       0.021638102269905,
						       -0.002423612146178};

	const sc_fixed<17,2,SC_TRN,SC_WRAP> coeff2[TAPS] = {-0.003143633003860,
						       0.027185874557551,
						       -0.125223235153495,
						       0.601180543048866,
						       0.601180543048866,
						       -0.125223235153495,
						       0.027185874557551,
						       -0.003143633003860};

	const sc_fixed<17,2,SC_TRN,SC_WRAP> coeff3[TAPS] = {-0.002423612146178,
						       0.021638102269905,
						       -0.107706831291022,
						       0.862310282276647,
						       0.287601741388967,
						       -0.077066788574370,
						       0.017755702728024,
						       -0.002109243232750};

	const sc_fixed<17,2,SC_TRN,SC_WRAP> coeff4[TAPS-1] = {-0.000000000617302,
						       0.000000003212201,
						       -0.000000007388632,
						       1.000000009584823,
						       -0.000000007388632,
						       0.000000003212201,
						       -0.000000000617302
						       };

	wait();

	while (1) {

		  // Read inputs
		indata_read = indata.read();
		  
		odata_en.write(0);

		// Read inputs by shifting previous data
		for ( n = TAPS-1; n>0; n--)
			buffer[n] = buffer[n-1];
		buffer[0] = indata_read;


		  // FIR 1 : Sum of Products of 1st filter
		SoP1 = 0;
		for (n = 0; n < TAPS; n++ ) 
			SoP1 = SoP1 + buffer[n] * coeff1[n];
		  
		  // FIR 2 : Sum of Products of 2nd filter
		SoP2 = 0;
		for (n = 0; n < TAPS; n++ ) 
			SoP2 =SoP2 + buffer[n] * coeff2[n];

		  // FIR 3 : Sum of Products of 3rd filter
		SoP3 = 0;
		for ( n = 0; n < TAPS; n++ ) 
			SoP3 = SoP3 + buffer[n] * coeff3[n];

		  // FIR 4 : Sum of Products of 4th filter
		SoP4 = 0;
		for (n = 0; n < TAPS-1; n++ ) 
			SoP4 = SoP4 + buffer[n] * coeff4[n];

		 // Write results back
		odata_en.write( 1 );
		odata[0].write(SoP1);
		odata[1].write(SoP2);
		odata[2].write(SoP3);
		odata[3].write(SoP4);
		  
		wait();
	}
}

