//========================================================================================
// 
// File Name    : tb_sobel.h
// Description  : Main testbench file
// Release Date : 23/02/2018
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra, Jianqi Chen
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version    Author            Description
//---------------------------------------------------------------------------------------
//23/07/2013      1.0     PolyU DARC Lab    sends,receives data and compares the result with the golden output     
//23/02/2018      1.1     UTD DARClab       fix some small bugs when sending data 
//=======================================================================================
#include "tb_sobel.h"

#include "define.h"
#include <time.h>
double elapsed;

//--------------------------
// Send data thread
//-------------------------
void tb_sobel::send(void){

  // Variables declration
  static int i,j,k;
 sc_uint<8> input_row_write[3];
  time_t end,start=time(NULL);

  // Initialization 
  bitmapData =  load_bitmapfile(IMAGE_IN);


  i=0;
  j=0;

   wait();

  while(true){

    // Send Image data to sobel filter
	for(k=0;k<BYTES_PER_PIXEL;k++){
		for(i=0;i<ROWS;i++){
			for(j=0;j<COLS;j+=BYTES_PER_PIXEL){
				if(i==0) input_row_write[0] = 0;
				else input_row_write[0] = bitmapData[((i-1)*COLS)+j+k];
				input_row[0].write(input_row_write[0]);

				input_row_write[1] = bitmapData[(i*COLS)+j+k];
				input_row[1].write(input_row_write[1]);

				if(i==(ROWS-1)) input_row_write[2] = 0;
				else input_row_write[2] = bitmapData[((i+1)*COLS)+j+k];
				input_row[2].write(input_row_write[2]);
			
				wait();
		
			}
		}
		input_row_write[0] = 0;
		input_row_write[1] = 0;
		input_row_write[2] = 0;
		wait();
		wait();
		wait();
	}

 
    cout << endl << "Starting comparing results " << endl;
 
    compare_results();  // Compare results with golden output 
    image_write();        // Generate bmp output file
    	end = time(NULL);
    	elapsed = difftime(end, start);
  	printf("\n\nRunning time %10.4f\n", elapsed);
    sc_stop();

  }//while loop ends
  
}


//
//  Bitmap loading function
//

 unsigned char  *tb_sobel::load_bitmapfile(const char *image)
{
 
  unsigned char *bitmapImage;
  int l;

 // Open bmp file to be filtered
  ifptr = fopen(image,"rb");
  if(!ifptr){
    cout << "Cannot open the file " << image << endl;
    sc_stop();
    exit(-1);
  }


  if(ifptr == NULL){
    sc_stop();
    exit (-1);
  }

 fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,ifptr);

#ifdef PRINTD
   printf("%c Bitmap identifies\n",bitmapFileHeader.bfType);
    printf("%d Bitmap identifies the size of file\n",bitmapFileHeader.bfSize);
#endif

 if (bitmapFileHeader.bfType !=0x4D42)
    {
        fclose(ifptr);
	
    }

 // Read the bitmap info header
 fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,ifptr);

 // Read colour palette
 fread(&biColourPalette,1,bitmapInfoHeader.biClrUsed*4,ifptr);

#ifdef PRINTD
    printf("\n%d the size of info header",bitmapInfoHeader.biSize);
    printf("\n%x the horizontal width",bitmapInfoHeader.biWidth);
    printf("\n%x the vertical height",bitmapInfoHeader.biHeight);
    printf("\n%x num of planes",bitmapInfoHeader.biPlanes);
    printf("\n%d bits per pixel",bitmapInfoHeader.biBitCount);
    printf("\n%d compression specs",bitmapInfoHeader.biCompression);
    printf("\n%d the size of the image",bitmapInfoHeader.biSizeImage);
    printf("\n%x the num of colours used",bitmapInfoHeader.biClrUsed);

#endif


    //move file point to the begging of bitmap data
    fseek(ifptr, bitmapFileHeader.bfOffBits, SEEK_SET);

    //allocate enough memory for the bitmap image data
    bitmapImage = (unsigned char*)malloc(bitmapFileHeader.bfSize);

    //verify memory allocation
    if (!bitmapImage)
      {
	cout << endl << "Cannot allocate memory for image" << endl;
	free(bitmapImage);
	return NULL;
	}

    //read in the bitmap image data
    fread(bitmapImage,1, bitmapInfoHeader.biSizeImage,ifptr);

    if (bitmapImage == NULL)
      {
	cout << endl << "Data could not be read" << endl;
	return NULL;
	}

#ifdef PRINTD
    cout << endl << "Finished reading bmp file" << endl;
  
#endif


    fclose(ifptr);
    return bitmapImage;
}


//--------------------------
// Receive data thread
//-------------------------
void tb_sobel::recv(void){

  // variablesd declaration
  sc_uint<8> output_row_read;
  int f=0,i,j,k;


  //open file for writing
  ofptr=fopen(IMAGE_OUT,"wb");

  bitmapFinalImage = (unsigned char *)malloc(bitmapFileHeader.bfSize);
  if (!bitmapFinalImage)
    {
      free(bitmapFinalImage);
      cout << endl << "Memory not allocated" << endl;  
    }

 

  while(true){

	for(k=0;k<BYTES_PER_PIXEL;k++){
		wait();
		wait();
		wait();
		for(i=0;i<ROWS;i++){
			for(j=0;j<COLS;j+=BYTES_PER_PIXEL){
			
				output_row_read = output_row.read();
				bitmapFinalImage[(i*COLS)+j+k] = output_row_read;
				wait();
		
			}
		}
	}

	  
  }//while loop ends

}



//--------------------------
// Compare results
//-------------------------
void tb_sobel::compare_results(){


  // Variables declaration
  int i,j, line =0, errors=0;

  // free memory of original image which contains un-filterd image.
  if(bitmapData != NULL)
    free(bitmapData);

  // Read Golden output image data
  bitmapData =  load_bitmapfile(IMAGE_GOLDEN);

  //Dump the comparison result
    diffptr = fopen (DIFFFILENAME, "w");
    if(!diffptr){
      cout << "Could not open " << DIFFFILENAME<< "\n";
      sc_stop();
      exit (-1);
    }
    

    // Main data comparison loop
    // for(i=0;i<ROWS;i++){
       for(j=0;j<SIZE/3;j++){

	 bitmapData[(i*ROWS)+j];
	  
#ifdef PRINTD 
	   cout << endl <<"Cycle["<< line << "]: " <<  bitmapData[(i*ROWS)+j] << "-- "<<  bitmapFinalImage[(i*ROWS)+j];
#endif

	   if( bitmapFinalImage[j] !=  bitmapData[j]){
	     cout << "\nOutput missmatch [line:" << line << "] Golden:" <<  std::hex  << bitmapData[j]  << " -- Output:" <<  std::hex  <<  bitmapFinalImage[j];
	     fprintf(diffptr,"\nOutput missmatch[line:%d] Golden: %x -- Output: %x",line,  bitmapData[j],  bitmapFinalImage[j]);	     
	     errors++;
	   }

          line ++;

	// }
     }

    if(errors == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errors << " MISSMATCHES between Golden and Simulation" << endl;


    fclose(diffptr);

}


/*
 ** Create new BMP file for filter results
*/


void tb_sobel::image_write(void){

  // Variables declaration
  int i,j,bytesperline,n;
  int l,k,len,m,x;
  unsigned char  *tk;


#ifdef PRINTD
  cout<< endl << "Image load function called" << endl;
#endif

 k=sizeof(BITMAPFILEHEADER);
 l=sizeof(BITMAPINFOHEADER);


 bytesperline = bitmapInfoHeader.biWidth * BYTES_PER_PIXEL; // multiply by 3 only for 24 bit images

 
 if( bytesperline & 0x0003)
   {  bytesperline |= 0x0003;
     ++bytesperline;}

#ifdef PRINTD

 cout << endl << "The size of file header object and info header object and final image " << k << l << endl;
 cout << endl << "The bytes per line " << bytesperline << endl;
#endif

 tk = (unsigned char  *)calloc(1,bytesperline);
 bitmapFileHeader.bfSize= bitmapInfoHeader.biSize + (long)bytesperline* bitmapInfoHeader.biHeight;

 bitmapFileHeader.bfOffBits = k+l+ 4* bitmapInfoHeader.biClrUsed;
 bitmapFileHeader.bfSize = k+l+bitmapInfoHeader.biSizeImage;

 
 n=0;

#ifdef PRINTD
 cout<< endl << "Printing to the bmp file" << endl;
  #endif

 if(ofptr!=NULL){
   fwrite(&bitmapFileHeader,1,sizeof(BITMAPFILEHEADER),ofptr);
   fwrite(&bitmapInfoHeader,1,sizeof(BITMAPINFOHEADER),ofptr);
   fwrite(biColourPalette,bitmapInfoHeader.biClrUsed*4,1,ofptr);

#ifdef PRINTD    
   cout << endl << "The bytes per line and k " << bytesperline << k; 
   cout << endl << "The size of info header " << bitmapInfoHeader.biSize;
   cout << endl << "The horizontal width " <<  std::hex  << bitmapInfoHeader.biWidth;
   cout << endl << "The vertical height " << std::hex << bitmapInfoHeader.biHeight ; 
   cout << endl << "Num of planes " << std::hex << bitmapInfoHeader.biPlanes;
   cout << endl << "Bits per pixel " << bitmapInfoHeader.biBitCount;
   cout << endl << "Compression specs " << bitmapInfoHeader.biCompression;
   cout << endl << "The size of the image " <<bitmapInfoHeader.biSizeImage;
   cout << endl << "The num of colours used " << std::hex << bitmapInfoHeader.biClrUsed;
#endif

   for(i=(bitmapInfoHeader.biHeight-1); i>=0; i--){  
       for(j=0;j<=(bytesperline)-1;j++){
	   tk[j] = bitmapFinalImage[n++];
	   }

       fwrite(tk,1,bytesperline,ofptr);
   }
   
#ifdef PRINTD
   cout << endl << "Writing of  image finished " << endl;
#endif      
 }
 else{
   cout << endl << "File not opened "<< endl;
 }


 fclose(ofptr);
 free(tk);

}
