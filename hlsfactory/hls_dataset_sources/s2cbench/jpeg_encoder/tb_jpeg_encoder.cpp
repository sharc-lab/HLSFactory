//========================================================================================
// 
// File Name    : tb_jpeg_encoder.cpp
// Description  : JPEG testbench 
// Release Date : 30/05/2017
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 30/05/2017        1.0        UTD DARClab     JPEG testbench
//=======================================================================================


#include "tb_jpeg_encoder.h"
#include "define.h"
#include <time.h>

double elapsed;

//--------------------------
// Send data thread
//-------------------------
void tb_jpeg_encoder::send(void){

  // Variables declration
  static int i,j,k,m;
  sc_uint<8> input_col_write[8];
  time_t end,start=time(NULL);

  // Initialization 
  unsigned char *bitmapData_r = load_bitmapfile(IMAGE_IN);
  // put the data upside down
  bitmapData = (unsigned char*) malloc(512*512);
  for(i=0;i<512;i++)
    for(j=0;j<512;j++)
      bitmapData[512*512-(i+1)*512+j] = bitmapData_r[i*512+j];


  while(true){

    // Send Image data to jpeg encoder
     for(i=0;i<(ROWS/8);i++){
         for(j=0;j<COLS;j++){
	   for(k=0;k<8;k++){
	
	     // Send 8 pixels at a time
	     input_col_write[k] = bitmapData[(i*COLS*8)+j+(k*COLS)];
	     input_col[k].write(input_col_write[k]);
  
	     
	   }
//	cout<<"sent"<<endl;
        wait();
	 }

     }
    for(m=0;m<7;m++)
      wait();


    image_write();        // Generate output file

    cout << endl << "Starting comparing results " << endl; 
    compare_results();  // Compare results with golden output 

    end = time(NULL);
    elapsed = difftime(end, start);
    printf("\n\nRunning time %10.4f\n", elapsed);
    sc_stop();

  }//while loop ends
  
}


//
//  Bitmap loading function
//

 unsigned char  *tb_jpeg_encoder::load_bitmapfile(const char *image)
{
 
  unsigned char *bitmapImage;
  int l;

 // Open jpg file
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
void tb_jpeg_encoder::recv(void){

  // variablesd declaration
  sc_uint<1> output_read[64];
  sc_uint<9> last_b;
  int k,m;


  //open file for writing
  ofptr=fopen(IMAGE_OUT,"wb");

  jpegFinalImage = (unsigned char *)malloc(512*512);
  jpegFinalImage_bits = (bool *)calloc(8*512*512,sizeof(bool));
  if (!jpegFinalImage||!jpegFinalImage_bits)
    {
      free(jpegFinalImage);
      free(jpegFinalImage_bits);
      cout << endl << "Memory not allocated" << endl;  
    }

  
  wait();
  jpeg_len = 0; //length of jpegFinalImage (in # of Bytes)
  jpeg_len_b = 0; //length of jpegFinalImage_bits (in # of bits)

  while(true){
    last_b = last_bit.read();

    if(last_b>=64){
      for(k=0;k<64;k++){
        output_read[k] = output[k].read();
        jpegFinalImage_bits[jpeg_len_b+k] = (bool)output_read[k];
      }
      jpeg_len_b += 64;
    }

    else if(last_b!=0){
      for(k=0;k<last_b;k++){
        output_read[k] = output[k].read();
        jpegFinalImage_bits[jpeg_len_b+k] = (bool)output_read[k];
      }
      jpeg_len_b += last_b;

    }

    wait();
	  
  }//while loop ends

}



//--------------------------
// Compare results
//-------------------------
void tb_jpeg_encoder::compare_results(){


  // Variables declaration
  int i,j, errors=0;
  unsigned char*  golden, * result;

  // Read Golden output image data
  ifptr = fopen("lena512_golden.jpg","rb");
  if(!ifptr){
    cout << "Could not open golden file"<< "\n";
    sc_stop();
    exit (-1);
  }
  fseek(ifptr,0,SEEK_END);
  int size = ftell(ifptr);
  golden = (unsigned char*) malloc(size);
  result = (unsigned char*) malloc(size);
  fseek(ifptr,0,SEEK_SET);
  fread(golden,1,size,ifptr);
  fclose(ifptr);

  // Read output result
  ifptr = fopen("lena512_out.jpg","rb");
  if(!ifptr){
    cout << "Could not open golden file"<< "\n";
    sc_stop();
    exit (-1);
  }
  fread(result,1,size,ifptr);
  fclose(ifptr);


  //Dump the comparison result
  diffptr = fopen (DIFFFILENAME, "w");
  if(!diffptr){
    cout << "Could not open " << DIFFFILENAME<< "\n";
    sc_stop();
    exit (-1);
  }
    

    // Main data comparison loop
  for(j=0;j<size;j++){ 
    if(result[j] !=  golden[j]){
//      cout << "\nGolden:" <<  hex  << golden[j]  << " -- Output:" <<  hex  <<  result[j];
      fprintf(diffptr,"\n Golden: %x -- Output: %x",  golden[j], result[j]);	     
      errors++;
    }
  }

    if(errors == 0)
      cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
      cout << endl << "Finished simulation " << errors << " MISSMATCHES between Golden and Simulation" << endl;


    fclose(diffptr);

}


/*
 ** Create new jpg file
*/


void tb_jpeg_encoder::image_write(void){

  // Variables declaration
  int i,j,k,p;
  unsigned char  *tk;
  unsigned char sof[13],soi[2], eoi[2], dqt[69], dhtDC[33], dhtAC[183], sos[10];

  //write SOI (start of image)
  soi[0] = 255;
  soi[1] = 216;
  fwrite(soi,1,2,ofptr);

  //write DQT (quantization table)
  dqt[0] = 255;
  dqt[1] = 219;  //marker
  dqt[2] = 0;   //length
  dqt[3] = 67;  //length
  dqt[4] = 0;   //8-bit Y component
  unsigned char Q[64] = {
    #include "quantization_matrix.dat"
  };

  for(j=0;j<64;j++)
    dqt[j+5] = Q[j];

  fwrite(dqt,1,69,ofptr);

  //write SOF (start of frame)
  sof[0] = 255;
  sof[1] = 192; //baseline sequential DCT mode
  sof[2] = 0;
  sof[3] = 11;//length
  sof[4] = 8; //# of bits for color value
  sof[5] = (unsigned char)(bitmapInfoHeader.biWidth/256);
  sof[6] = (unsigned char)(bitmapInfoHeader.biWidth%256);
  sof[7] = (unsigned char)(bitmapInfoHeader.biHeight/256);
  sof[8] = (unsigned char)(bitmapInfoHeader.biHeight%256);
  sof[9] = 1; //# of components (grayscale=1)
  sof[10] = 0; // component identifier
  sof[11] = 17; // 0b00010001 (H,V)=(1,1)
  sof[12] = 0; //quantization table destination selector
  fwrite(sof,1,13,ofptr);
  
  //write DHT (DC Huffman table)
  dhtDC[0] = 255;
  dhtDC[1] = 196;
  dhtDC[2] = 0;
  dhtDC[3] = 31;  //length
  dhtDC[4] = 0;  //DC, Y component
  unsigned char BITS_DC[16] = {0,1,5,1,1,1,1,1,1,0,0,0,0,0,0,0};
  for(k=0;k<16;k++)
    dhtDC[k+5] = BITS_DC[k];

  unsigned char HUFFVAL_DC[12] =  {3,0,1,2,4,5,6,7,8,9,10,11};
  for(k=0;k<12;k++)
    dhtDC[k+21] = HUFFVAL_DC[k];

  fwrite(dhtDC,1,33,ofptr);

  //write DHT (AC Huffman table)
  dhtAC[0] = 255;
  dhtAC[1] = 196;
  dhtAC[2] = 0;
  dhtAC[3] = 181; //length
  dhtAC[4] = 16; // 0b00010000
  unsigned char BITS_AC[16] = {0,2,1,3,2,4,4,3,5,5,5,2,0,0,1,125};
  for(k=0;k<16;k++)
    dhtAC[k+5] = BITS_AC[k];

  unsigned char HUFFVAL_AC[162] = 
{1, 2, 3, 0, 4, 17, 5, 33, 6, 18, 49, 65, 7, 19, 81, 97, 34, 113, 129, 20, 50, 145, 161, 177, 8, 35, 66, 82, 193, 21, 51, 98, 114, 209, 225, 240, 130, 9, 10, 22, 23, 24, 25, 26, 36, 37, 38, 39, 40, 41, 42, 52, 53, 54, 55, 56, 57, 58, 67, 68, 69, 70, 71, 72, 73, 74, 83, 84, 85, 86, 87, 88, 89, 90, 99, 100, 101, 102, 103, 104, 105, 106, 115, 116, 117, 118, 119, 120, 121, 122, 131, 132, 133, 134, 135, 136, 137, 138, 146, 147, 148, 149, 150, 151, 152, 153, 154, 162, 163, 164, 165, 166, 167, 168, 169, 170, 178, 179, 180, 181, 182, 183, 184, 185, 186, 194, 195, 196, 197, 198, 199, 200, 201, 202, 210, 211, 212, 213, 214, 215, 216, 217, 218, 226, 227, 228, 229, 230, 231, 232, 233, 234, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250};

  for(k=0;k<162;k++)
    dhtAC[k+21] = HUFFVAL_AC[k];

  fwrite(dhtAC,1,183,ofptr);

  //write SOS (start of scan)
  sos[0] = 255;
  sos[1] = 218;
  sos[2] = 0;
  sos[3] = 8;
  sos[4] = 1; //# of components
  sos[5] = 0; //Y component identifier
  sos[6] = 0; //DC,AC selector
  sos[7] = 0;
  sos[8] = 63;
  sos[9] = 0;
  fwrite(sos,1,10,ofptr);

  //write data
  jpeg_len = int(jpeg_len_b/8)+1;
  //put 1's in remainder of the last byte
  for(p=0;p<7;p++)
    jpegFinalImage_bits[jpeg_len_b] = 1;
  for(p=0;p<jpeg_len;p++){
    jpegFinalImage[p] = bitToByte(&jpegFinalImage_bits[p*8]);
  }

  tk = (unsigned char  *)calloc(jpeg_len,sizeof(unsigned char));

  if(ofptr!=NULL){
    for(i=0;i<jpeg_len;i++){
      tk[i] = jpegFinalImage[i];
    }
   
    fwrite(tk,1,jpeg_len,ofptr);
  }

  else{
    cout << endl << "File not opened "<< endl;
  }


  //write EOI (end of image)
  eoi[0] = 255;
  eoi[1] = 217;
  fwrite(eoi,1,2,ofptr);

 fclose(ofptr);
 free(tk);

}

unsigned char tb_jpeg_encoder::bitToByte(bool* bitarray)
{
  unsigned char ret = 0;
  for(int i=0; i<8; ++i){
    if(bitarray[i])
      ret |= 1 << (7-i);
  }
  return ret;
}

