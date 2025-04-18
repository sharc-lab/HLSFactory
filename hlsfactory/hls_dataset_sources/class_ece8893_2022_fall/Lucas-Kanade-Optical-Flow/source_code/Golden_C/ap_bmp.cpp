/*******************************************************************************
Vendor: Xilinx 
Associated Filename: ap_bmp.cpp
Purpose: BMP image reader and writer for Vivado HLS   
Revision History: February 13, 2012 - initial release
                                                
*******************************************************************************
© Copyright 2008 - 2012 Xilinx, Inc. All rights reserved. 

This file contains confidential and proprietary information of Xilinx, Inc. and 
is protected under U.S. and international copyright and other intellectual 
property laws.

DISCLAIMER
This disclaimer is not a license and does not grant any rights to the materials 
distributed herewith. Except as otherwise provided in a valid license issued to 
you by Xilinx, and to the maximum extent permitted by applicable law: 
(1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND XILINX 
HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, 
INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR 
FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether 
in contract or tort, including negligence, or under any other theory of 
liability) for any loss or damage of any kind or nature related to, arising under 
or in connection with these materials, including for any direct, or any indirect, 
special, incidental, or consequential loss or damage (including loss of data, 
profits, goodwill, or any type of loss or damage suffered as a result of any 
action brought by a third party) even if such damage or loss was reasonably 
foreseeable or Xilinx had been advised of the possibility of the same.

CRITICAL APPLICATIONS
Xilinx products are not designed or intended to be fail-safe, or for use in any 
application requiring fail-safe performance, such as life-support or safety 
devices or systems, Class III medical devices, nuclear facilities, applications 
related to the deployment of airbags, or any other applications that could lead 
to death, personal injury, or severe property or environmental damage 
(individually and collectively, "Critical Applications"). Customer assumes the 
sole risk and liability of any use of Xilinx products in Critical Applications, 
subject only to applicable laws and regulations governing limitations on product 
liability. 

THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT 
ALL TIMES.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ap_bmp.h"



//Open the BMP input file
FILE *BMP_InputOpen(char *name)
{
  FILE *bmp_file = fopen(name,"rb");
  if(!bmp_file){
    printf("ERROR: could not open %s for reading\n",name);
  }
//  else
    return bmp_file;
}

//Open the BMP output file
FILE *BMP_OutputOpen(char *name){
  FILE *bmp_file = fopen(name,"wb");
  if(!bmp_file){
    printf("ERROR: could not open %s for writing\n",name);
  }
//  else
    return bmp_file;
}

//Close BMP file
int BMP_Close(FILE *bmp_file){
  if(bmp_file){
    fclose(bmp_file);
    return 0;
  }
  else{
    printf("ERROR: could not close bmp file\n");
    return 1;
  }
}

//Read the BMP file header
int BMP_Read_FileHeader(FILE *bmp_file, BMPHeader *file_header){
  if(!bmp_file){
    printf("ERROR: Can't read the image file\n");
    return 1;
  }

  if(!fread(file_header,1,14,bmp_file)){
    printf("ERROR: Failed to read the image file header \n");
    return 1;
  }
  else
    return 0;
}

//Read the BMP data header
int BMP_Read_ImageHeader(FILE *bmp_file, BMPImageHeader *image_header){
  if(!bmp_file){
    printf("ERROR: Can't read the image file\n");
    return 1;
  }

  if(!fread(image_header,1,40,bmp_file)){
    printf("ERROR: Failed to read the image data header \n");
    return 1;
  }
  else
    return 0;
}

//Read the BMP image data
int BMP_Read_Image(FILE *bmp_file, unsigned char *data, int size){
   if(!bmp_file){
    printf("ERROR: Can't read the image file\n");
    return 1;
  }

   if(!fread(data,1,size,bmp_file)){
      printf("ERROR: Can't read the image data\n");
    return 1;
   }
   else{
     return 0;
   }
}

//Blank BMP Image
BMPImage *BMP_CreateBlank(void)
{
  BMPImage *bitmap = NULL;
  BMPHeader *file_header = NULL;
  BMPImageHeader *image_header = NULL;

  unsigned char *data;

  file_header = (BMPHeader *) calloc(sizeof(BMPHeader),1);
  if(!file_header){
    printf("ERROR: Can't create file header in blank BMP\n");
    return NULL;
  }

  image_header = (BMPImageHeader *) calloc(sizeof(BMPImageHeader),1);
  if(!image_header){
    printf("ERROR: Can't create image header in blank BMP\n");
    if(file_header) free(file_header);
    return NULL;
  }

  bitmap = (BMPImage *) malloc(sizeof(BMPImage));
  if(!bitmap){
    printf("ERROR: Can't create image data in blank BMP\n");
    if(file_header) free(file_header);
    if(image_header) free(image_header);
    return NULL;
  }
  
  data = (unsigned char *) malloc(MAX_COLS*MAX_ROWS*3);
  if(!data){
    printf("ERROR: Allocate image temp space\n");
    if(file_header) free(file_header);
    if(image_header) free(image_header);
    if(bitmap) free(bitmap);
    return NULL;
  }
  
  bitmap->file_header = file_header;
  bitmap->image_header = image_header;
  bitmap->data = data;

  //Initialize to 0
  int i, j;
  for(i = 0; i < MAX_ROWS; i++){
    for(j= 0; j < MAX_COLS; j++){
      bitmap->R[i][j]=0;
      bitmap->G[i][j]=0;
      bitmap->B[i][j]=0;
      bitmap->Y[i][j]=0;
      bitmap->U[i][j]=0;
      bitmap->V[i][j]=0;
    }
  }

  return bitmap;
}

//Delete bitmap from memory
void BMP_Delete(BMPImage *bitmap)
{
  if(bitmap){
    if(bitmap->file_header) free(bitmap->file_header);
    if(bitmap->image_header) free(bitmap->image_header);
    if(bitmap->data) free(bitmap->data);
    free(bitmap);
  }
}


//Read the image
int BMP_Read(char *file, int row, int col, unsigned char *R, unsigned char *G, unsigned char *B){
  BMPHeader *file_header = NULL;
  BMPImageHeader *image_header = NULL;
  unsigned char *image_data = NULL;

  FILE *bmp_file = NULL;

  bmp_file = BMP_InputOpen(file);

  if(!bmp_file){
    printf("ERROR: Can't open file %s\n",file);
    return 1;
  }

  file_header = (BMPHeader *)malloc(sizeof(BMPHeader));
  if(!file_header){
      printf("ERROR: Can't allocate memory for file header\n");
    return 1;
  }

  image_header = (BMPImageHeader *)malloc(sizeof(BMPImageHeader));
  if(!image_header){
      printf("ERROR: Can't allocate memory for image header\n");
      if(file_header) free(file_header);
    return 1;
  }

  int header_read;
  header_read = BMP_Read_FileHeader(bmp_file,file_header);

  if(header_read){
    printf("ERROR: Can't read the image header\n");
    if(file_header) free(file_header);
    if(image_header) free(image_header);
    return 1;
  }

  int img_header_read;

  img_header_read = BMP_Read_ImageHeader(bmp_file,image_header);
  if(img_header_read){
    printf("ERROR: Can't read the data header\n");
    if(file_header) free(file_header);
    if(image_header) free(image_header);
    return 1;
  }
  
  //Allocate memory for the image pixels
  image_data = (unsigned char *)malloc(image_header->Height * image_header->Width * 3);
  if(!image_data){
    printf("ERROR: Can't allocate memory for the image\n");
    if(file_header) free(file_header);
    if(image_header) free(image_header);
    return 1;
  }

  int img_data_read;
  
  img_data_read = BMP_Read_Image(bmp_file,image_data,(image_header->Height*image_header->Width*3));
  if(img_data_read){
    printf("ERROR: Can't read the image data\n");
    if(file_header) free(file_header);
    if(image_header) free(image_header);
    if(image_data) free(image_data);
    return 1;
  }

  if(row != (int)image_header->Height){
    printf("ERROR: file image height %i different from requested height %i\n",image_header->Height,row);
    if(file_header) free(file_header);
    if(image_header) free(image_header);
    if(image_data) free(image_data);
    return 1;
  }

  if(col != (int)image_header->Width){
    printf("ERROR: file image height %i different from requested height %i\n",image_header->Width,col);
    if(file_header) free(file_header);
    if(image_header) free(image_header);
    if(image_data) free(image_data);
    return 1;
  }

  unsigned char *tmp = image_data;

  //Copy the image data into the storage arrays
  int i, j;
  for(i=0; i < row; i++){
    for(j=0; j < col; j++){
      unsigned char r_temp, b_temp, g_temp;

      b_temp = *tmp++;
      g_temp = *tmp++;
      r_temp = *tmp++;

      //      printf("R %d G %d B %d\n",r_temp,b_temp,g_temp);

      R[(row-1-i)*col+j] = r_temp;
      G[(row-1-i)*col+j] = g_temp;
      B[(row-1-i)*col+j] = b_temp;
    }
  }

  BMP_Close(bmp_file);

  if(file_header) free(file_header);
  if(image_header) free(image_header);
  if(image_data) free(image_data);

  return 0;
}

//Write the image to a file
int BMP_Write(char *file, int row, int col, unsigned char *R, unsigned char *G, unsigned char *B)
{
  BMPImage *bitmap = NULL;
  FILE *output_image;
  unsigned char *data;

  bitmap = BMP_CreateBlank();
  if(!bitmap){
    printf("ERROR: Can't create image for output file\n");
    return 1;
  }
  
  bitmap->file_header->FileType = 19778;
  bitmap->file_header->FileSize = 0;
  bitmap->file_header->Reserved1 = 0;
  bitmap->file_header->Reserved2 = 0;
  bitmap->file_header->Offset = 54;

  bitmap->image_header->Size = 40;
  bitmap->image_header->Width = col;
  bitmap->image_header->Height = row;
  bitmap->image_header->Planes = 1;
  bitmap->image_header->BitsPerPixel = 24;
  bitmap->image_header->Compression = 0;
  bitmap->image_header->SizeOfBitmap = 3*row*col;
  bitmap->image_header->HorzResolution = 2835;
  bitmap->image_header->VertResolution = 2835;
  bitmap->image_header->ColorsUsed = 0;
  bitmap->image_header->ColorsImportant = 0;

  data = bitmap->data;
  
  //Store the image, bitmaps are stores upside down
  int i, j;
  for(i = 0; i < row; i++){
    for(j=0; j < col; j++){
      unsigned char r_temp, g_temp, b_temp;
      
      r_temp = R[(row-1-i)*col+j];
      g_temp = G[(row-1-i)*col+j];
      b_temp = B[(row-1-i)*col+j];

      //   printf("i %d j %d R %d G %d B %d\n",i, j,r_temp,g_temp,b_temp);

      *data++ = b_temp;
      *data++ = g_temp;
      *data++ = r_temp;
    }
  }

  output_image = fopen(file,"wb");
  if(!output_image){
    printf("ERROR: Can't open %s for writing\n",file);
    BMP_Delete(bitmap);
    return 1;
  }

  fwrite(&bitmap->file_header->FileType,1,2,output_image);
  fwrite(&bitmap->file_header->FileSize,1,4,output_image);
  fwrite(&bitmap->file_header->Reserved1,1,2,output_image);
  fwrite(&bitmap->file_header->Reserved2,1,2,output_image);
  fwrite(&bitmap->file_header->Offset,1,4,output_image);

  fwrite(&bitmap->image_header->Size,1,4,output_image);
  fwrite(&bitmap->image_header->Width,1,4,output_image);
  fwrite(&bitmap->image_header->Height,1,4,output_image);
  fwrite(&bitmap->image_header->Planes,1,2,output_image);
  fwrite(&bitmap->image_header->BitsPerPixel,1,2,output_image);
  fwrite(&bitmap->image_header->Compression,1,4,output_image);
  fwrite(&bitmap->image_header->SizeOfBitmap,1,4,output_image);
  fwrite(&bitmap->image_header->HorzResolution,1,4,output_image);
  fwrite(&bitmap->image_header->VertResolution,1,4,output_image);
  fwrite(&bitmap->image_header->ColorsUsed,1,4,output_image);
  fwrite(&bitmap->image_header->ColorsImportant,1,4,output_image);

  fwrite(bitmap->data,1,bitmap->image_header->Height*bitmap->image_header->Width*3,output_image);

  fclose(output_image);
  BMP_Delete(bitmap);

  return 0;
}
  
