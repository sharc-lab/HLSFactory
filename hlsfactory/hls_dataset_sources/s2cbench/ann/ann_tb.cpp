//========================================================================================
// 
// File Name    : ann_tb.cpp
// Description  : ANN Test bench
// Release Date : 22/10/2014
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 2014          1.0            David Aledo     top module (TB and ANN)
// 2014          1.1            B Carrion Schafer  added 2 and 4 layer configuration
//=======================================================================================
#include "ann_tb.h"


/*
 ** Constructor
 */
ann_tb::ann_tb(sc_module_name nm) : sc_module(nm)
{
   bmp_file_in = 0;
   bmp_file_out= 0;
   nn = new layer[Nlayer];
   for(int i=0; i<Nlayer; i++)
   {
      nn[i].initialize(NumN[i], NumN[i+1]);
   }

   SC_CTHREAD(learning_cthread, clk.pos());
      reset_signal_is(rst,true); // Asynchronous
}


/*
 ** Destructor
 */
ann_tb::~ann_tb()
{
   delete [] nn;
}

/*
 ** ERROR calculation
 */
double  ann_tb::error_calculation(int *P, double *e)
{
   double MSE = 0.0; // Mean Squared Error
   double aux;
   int i,  Local_e;

   for(i = 0; i < nn[Nlayer-1].NumN; i++)
   {
      Local_e = (P[i] - nn[Nlayer-1].a[i]); // differences between image and trained set
      aux = Local_e * Local_e;
      MSE += aux;
      e[i] = Local_e;
   }
   return MSE;
}


/*
 ** ANN learner main function
 */
void ann_tb::learning_cthread()
{
   /* Variables */
   int *P = new int[nn[0].NumIn];  // Input patch. It is also equal to the desired output patch
   double *e = new double[nn[Nlayer-1].NumN]; // Error between output and desired output
   bool worse;
   FILE *ptr_file_results = NULL;

   time_t end,  start=time(NULL);
   double elapsed;

   ptr_file_results = fopen("ann_log.rpt", "wt");
   if(ptr_file_results == NULL)
   {
     cout << "Cannot create log file ann_log.rpt" << endl;
     sc_stop();
   }

   double Lr;

   int ContAp;
   int ContIt;


   double TMSE; // Total Mean Squared Error
   double TMSE_Min; // Minimal TMSE achieved
   double PSNR; // Peak signal to noise ratio

   /* Reset initialization */
   wr->write(false);
   worse = false;
   Lr = Lr_START;
   TMSE_Min = 2e82; //Initialized to a big number
   ContAp = 0;
   ContIt = 0;


   image = load_bitmapfile(bmp_file_in); // Open the input image file and store it in image array


   int H = bitmapInfoHeader.biWidth;
   int V = bitmapInfoHeader.biHeight;
   N_tr_block = (H/baseB)*(V/baseB); 

   wait();

   /* 
    ** Weight and bias RANDOM initialization
    */
   wr->write(true);
   srand (time(NULL)); // initialize random seed
   for(int l=0; l<Nlayer; l++)
   {
      /* Generate RANDOM weights first */
      for(int n=0; n<nn[l].NumN; n++)
      {
         for(int i=0; i<nn[l].NumIn; i++)
         {
            nn[l].W[n][i] = (rand()%RAND_RANGE) - RAND_RANGE/2;
            wdata->write(nn[l].W[n][i]);
            wait();
         }
      }
      /* Generate RANDOM biases second */
      for(int n=0; n<nn[l].NumN; n++)
      {
         nn[l].b[n] = (rand()%RAND_RANGE) - RAND_RANGE/2;
         wdata->write(nn[l].b[n]);
         wait();
      }
   }
   wr->write(false);

   wait();

   /*******************
    **
    ** Main loop
    **
    *******************/
   do
   {
      TMSE = 0;

      // <-----Start ANN computation for each training image block 
      for (int i_main = 0; i_main < N_tr_block; i_main++)
      {

         // Send P to the ANN
         // Next image block
         for (int i=0; i<baseB; i++)
         {
            for (int j=0; j<baseB; j++)
            {
               P[baseB*i+j] = image[(i+(i_main/(H/baseB))*baseB)*H + j+(i_main%(H/baseB))*baseB]-128;
               inputs[baseB*i+j]->write(P[baseB*i+j]);
            }
         }
         run_in->write(true); 
         wait();
         run_in->write(false);

         while(!run_out->read()) wait(); // Waiting for the ANN answer

         /*
          ** FEEDFORWARDS
          */
         for(int l=0; l<(Nlayer-1); l++)
         {
            for(int i=0; i<nn[l].NumN; i++)
            {
               nn[l].a[i] = a[l][i];
            }
         }
         for(int i=0; i<nn[Nlayer-1].NumN; i++)
         {
            nn[Nlayer-1].a[i] = outputs[i]->read();
         }
         wait();


         /*
          ** ERROR CALCULATION
          */
         TMSE += error_calculation(P, e);

         /*
          **BACKPROPAGATION
          */
         // Calculation of DW and Db of the output layer
         for(int i = 0; i < nn[Nlayer-1].NumN; i++)
         {
            // Calculation of sensibility S of the output layer
            double Local_S2 = -2.0;
            Local_S2 *= e[i];

            nn[Nlayer-1].s[i] =  Local_S2;
            Local_S2 = Local_S2*Lr;
            nn[Nlayer-1].Db[i] += Local_S2;


            for(int j = 0; j < nn[Nlayer-1].NumIn; j++)
            {
               nn[Nlayer-1].DW[i][j] += Local_S2 * nn[Nlayer-1].a[j];
            };
         };

         // Calculation of DW and Db of the rest layers
         for(int l=(Nlayer-2); l>=0; l--)
         {
            for(int i = 0; i < nn[l].NumN; i++)
            {
               // Calculation of sensibility S
               double Local_S1 = 0.0;
               // S1 = F1*W2'*S2;
               for (int j = 0; j < nn[l+1].NumN; j++)
               {
                  Local_S1 += nn[l+1].W[j][i]*nn[l+1].s[j]*0.000244140625; //*1/(2^12)
               };

               Local_S1 *= derived_saturated(nn[l].a[i]);
	     
               nn[l].s[i] = Local_S1;
               Local_S1 = Local_S1*Lr;
               nn[l].Db[i] += Local_S1;

               // DW1 = Lr*S1*P';
               // As S2 = S2*Lr => DW1 = S1*P
               for(int j = 0; j < nn[l].NumIn; j++)
               {
                  if(l==0)
                     nn[l].DW[i][j] += Local_S1*((double)P[j]);
                  else
                     nn[l].DW[i][j] += Local_S1*((double)nn[l-1].a[j]);
               };
            }; // End of calculation of DW and Db
         }
      }
      
      // ----->Finished ANN computation

      // <-----Start calculating the QoR (ERROR)    
      TMSE = TMSE/(H*V);

      if (TMSE > TMSE_Min*Nu_ERROR)
      {
         // It is not converging
         worse = true;
         Lr    = Lr*Ro_DECREASE;
      }
      else if (TMSE <= TMSE_Min) //PF 13+0
      {
         // It is converging
         Lr = Lr*Ephsylon_INCREASE;
         TMSE_Min   = TMSE;
      };
   
      // -----> Finished calculating the QoR (ERROR)  

      /*
       ** Achieved better or worse results as before
       **
       ** If better ==> updating of weights, online
       */
      if(!worse) // Results were better -> Update
      {
         int clipp;
         for(int l=0; l<Nlayer; l++)
         {
            for(int i=0; i<nn[l].NumN; i++)
            {
               nn[l].bprev[i] = nn[l].b[i];
               clipp = nn[l].b[i] - ((int)(nn[l].Db[i]*4.0));
               if (clipp >= 1<<(NbitW-1)) //Clipping
                  nn[l].b[i] = (1<<(NbitW-1))-1; //0x00007FFF for 16 bits;
               else if (clipp <= -1<<(NbitW-1))
                  nn[l].b[i] = -1<<(NbitW-1); //0xFFFF8000 for 16 bits;
               else
                  nn[l].b[i] = clipp;
               for(int j=0; j<nn[l].NumIn; j++)
               {
                  nn[l].Wprev[i][j] = nn[l].W[i][j];
                  clipp = nn[l].W[i][j] - ((int)(nn[l].DW[i][j]*4.0));
                  if (clipp >= 1<<(NbitW-1)) //Clipping
                     nn[l].W[i][j] = (1<<(NbitW-1))-1; //0x00007FFF for 16 bits;
                  else if (clipp <= -1<<(NbitW-1))
                     nn[l].W[i][j] = -1<<(NbitW-1); //0xFFFF8000 for 16 bits;
                  else
                     nn[l].W[i][j] = clipp;
                  nn[l].DW[i][j] = 0.0;
               }
               nn[l].Db[i] = 0.0;
            }
         }
         ContAp++; // no worse iteration
      }
      /*
       ** If worse => restore previous values
       */
      else
      {
         for(int l=0; l<Nlayer; l++)
         {
            for(int i=0; i<nn[l].NumN; i++)
            {
               nn[l].b[i] = nn[l].bprev[i];
               for(int j=0; j<nn[l].NumIn; j++)
               {
                  nn[l].W[i][j] = nn[l].Wprev[i][j];
                  nn[l].DW[i][j] = 0.0;
               }
               nn[l].Db[i] = 0.0;
            }
         }
         worse = false;
      }

      /* Write weights and biases */
      wr->write(true);
      for(int l=0; l<Nlayer; l++)
      {
         /* Write weights first */
         for(int n=0; n<nn[l].NumN; n++)
         {
            for(int i=0; i<nn[l].NumIn; i++)
            {
               wdata->write(nn[l].W[n][i]);
               wait();
            }
         }
         /* Write biases second */
         for(int n=0; n<nn[l].NumN; n++)
         {
            wdata->write(nn[l].b[n]);
            wait();
         }
      }

      wr->write(false);
      wait();

      ContIt++;

    // Exit condition. 
   }while(TMSE > GOAL_ERROR && ContAp < N_ITER);


   end = time(NULL);
   elapsed = difftime(end, start);

   PSNR = 10*log10( (255*255) / TMSE ); 	//Peak Signal to Noise Ratio (PSNR)

   cout << "TMSE: " << TMSE << endl;
   cout << "PSNR: " << PSNR << endl;
   cout << "N_ITER: " << ContAp << endl;
   cout << "BMP: "<< bmp_file_in << endl << endl; 

   fprintf(ptr_file_results,"TMSE: %0.4lf", TMSE);
   fprintf(ptr_file_results,"\nPSNR: %0.4lf", PSNR);
   fprintf(ptr_file_results,"\nN_ITER: %d", N_ITER);
   fprintf(ptr_file_results,"\nTRAIN_TIME: %0.2lf", elapsed);
   fprintf(ptr_file_results,"\nBMP: %s", bmp_file_in); 


   /***************
    **
    ** Run the ANN a last time to store the final result image
    **
    ***********/

   result = new unsigned char[bitmapInfoHeader.biSizeImage];
   for (int i_main = 0; i_main < N_tr_block; i_main++)
   {
      for (int i=0; i<baseB; i++)
      {
         for (int j=0; j<baseB; j++)
         {
            P[baseB*i+j] = image[(i+(i_main/(H/baseB))*baseB)*H + j+(i_main%(H/baseB))*baseB]-128;
            inputs[baseB*i+j]->write(P[baseB*i+j]);
         }
      }
      run_in->write(true);
      wait();
      run_in->write(false); 

      while(!run_out->read()) wait();

      for (int i=0; i<baseB; i++)
      {
         for (int j=0; j<baseB; j++)
         {
            result[(i+(i_main/(H/baseB))*baseB)*H + j+(i_main%(H/baseB))*baseB] = outputs[baseB*i+j]->read()+128;
         }
      }
      wait();

   }

   /* Store the final result image in BMP file */
   image_write(bmp_file_out);


   /************************************
    **
    ** Verification of test results
    **
    *************************************/
   delete [] image; // Free memory from the training image
   image = load_bitmapfile(bmp_file_in_check);

   H = bitmapInfoHeader.biWidth;
   V = bitmapInfoHeader.biHeight;
   N_tr_block = (H/baseB)*(V/baseB);
   TMSE = 0.0;
   delete [] result; // Free memory from the training result
   result = new unsigned char[bitmapInfoHeader.biSizeImage];
   for (int i_main = 0; i_main < N_tr_block; i_main++)
   {
      for (int i=0; i<baseB; i++)
      {
         for (int j=0; j<baseB; j++)
         {
            P[baseB*i+j] = image[(i+(i_main/(H/baseB))*baseB)*H + j+(i_main%(H/baseB))*baseB]-128;
            inputs[baseB*i+j]->write(P[baseB*i+j]);
         }
      }
      run_in->write(true); 
      wait();
      run_in->write(false);

      while(!run_out->read()) wait();

      for (int i=0; i<baseB; i++)
      {
         for (int j=0; j<baseB; j++)
         {
            nn[Nlayer-1].a[i*baseB+j] = outputs[baseB*i+j]->read();
            result[(i+(i_main/(H/baseB))*baseB)*H + j+(i_main%(H/baseB))*baseB] = nn[Nlayer-1].a[i*baseB+j] + 128;
         }
      }
      wait();
      TMSE += error_calculation(P, e);
   }

   TMSE = TMSE/(H*V);
   PSNR = 10*log10( (255*255) / TMSE ); 	//Peak Signal to Noise Ratio (PSNR)
   cout << "TMSE_TEST: " << TMSE << endl;
   cout << "PSNR_TEST: " << PSNR << endl;
   cout << "BMP_TEST: "<< bmp_file_in_check << endl << endl;

   fprintf(ptr_file_results,"\nTMSE_TEST: %0.4lf", TMSE);
   fprintf(ptr_file_results,"\nPSNR_TEST: %0.4lf", PSNR);
   fprintf(ptr_file_results,"\nBMP_TEST: %s", bmp_file_in_check);

   /* Store the final result image in BMP file */
   image_write(bmp_file_out_check);

#ifdef PRINTD
   /*  
    ** Record the weights and bias
    */
   fprintf(ptr_file_results,"\n\nWeights&Bias:"); 
   for(int l=0; l<Nlayer; l++)
   {
       fprintf(ptr_file_results,"\nLayer %d ",l);
       fprintf(ptr_file_results,"\nWeights:");

       /* Write weights first */
       for(int n=0; n<nn[l].NumN; n++)
       {
          fprintf(ptr_file_results,"\n");
          for(int i=0; i<nn[l].NumIn; i++)
          {
             fprintf(ptr_file_results,"%d ",nn[l].W[n][i]);
          }
       }
       /* Write biases second */
       fprintf(ptr_file_results,"\nBias:\n");
       for(int n=0; n<nn[l].NumN; n++)
       {
	       fprintf(ptr_file_results,"%d ",nn[l].b[n]);
       }
   }
   fprintf(ptr_file_results,"\n\n");
#endif

   fclose(ptr_file_results);

   delete [] image;
   delete [] result;
   delete [] e;
   delete [] P;

   cout << "End of simulation." << endl;
   exit(1);
//sc_stop();
}


/******************************
 **
 **  Saturate max values given by NbitOut
 **
 *******************************/
double ann_tb::derived_saturated(int x)
{

  int max= pow(2,NbitOut-1);
  int step = max >> 3;
  if ((x == -(max)) || (x == max-1))
      return 0;
   else if ((x < -(max-step)) || (x > (max-step)))
      return 0.0625;
   else if ((x < -(max-2*step)) || (x > (max-2*step)))
      return 0.125;
   else if ((x < -(max-3*step)) || (x > (max-3*step)))
      return 0.25;
   else if ((x < -(max-4*step)) || (x > (max-4*step)))
      return 0.5;
   else
      return 1;
} 


/******************************
 **
 **  Bitmap loading function
 **
 *******************************/
unsigned char  *ann_tb::load_bitmapfile(const char *im_path)
{
   unsigned char *bitmapImage;
   int l;

   // Open bmp file to be filtered
   image_in = fopen(im_path,"rb");
   if(!image_in)
   {
      cout << "Cannot open the file " << im_path << endl;
      sc_stop();
      exit(-1);
   }

   fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,image_in);

#ifdef PRINTD
   cout << bitmapFileHeader.bfType << " Bitmap identifies" << endl;
   cout << bitmapFileHeader.bfSize << " Bitmap identifies the size of file" << endl;
#endif

   if(bitmapFileHeader.bfType !=0x4D42)
   {
      fclose(image_in);
   }

   // Read the bitmap info header
   fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,image_in);

   // Read colour palette
   fread(&biColourPalette,1,bitmapInfoHeader.biClrUsed*4,image_in);

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
   fseek(image_in, bitmapFileHeader.bfOffBits, SEEK_SET);

   //allocate enough memory for the bitmap image data
   bitmapImage = new unsigned char[bitmapInfoHeader.biSizeImage];

   //verify memory allocation
   if(!bitmapImage)
   {
      cout << endl << "Cannot allocate memory for image" << endl;
      //delete [] bitmapImage;
      return NULL;
   }

   //read in the bitmap image data
   fread(bitmapImage,1, bitmapInfoHeader.biSizeImage,image_in);

   if (bitmapImage == NULL)
   {
      cout << endl << "Data could not be read" << endl;
      return NULL;
   }

#ifdef PRINTD
    cout << endl << "Finished reading bmp file" << endl;
#endif

    fclose(image_in);
    return bitmapImage;
}


/********************************************
 **
 ** Create new BMP file for filter results 
 **
 *********************************************/
void ann_tb::image_write(char *image_out_name)
{
   // Variables declaration
   int i,j,bytesperline,n;
   int l,k,len,m,x;
   unsigned char *tk;

#ifdef PRINTD
   cout<< endl << "Image load function called" << endl;
#endif

   k = sizeof(BITMAPFILEHEADER);
   l = sizeof(BITMAPINFOHEADER);

   bytesperline = bitmapInfoHeader.biWidth * BYTES_PER_PIXEL; // multiply by 3 only for 24 bit images

   if( bytesperline & 0x0003)
   {
      bytesperline |= 0x0003;
      ++bytesperline;
   }

#ifdef PRINTD
   cout << endl << "The size of file header object and info header object and final image " << k << l << endl;
   cout << endl << "The bytes per line " << bytesperline << endl;
#endif

   tk = (unsigned char  *)calloc(1,bytesperline);
   bitmapFileHeader.bfSize= bitmapInfoHeader.biSize + (long)bytesperline*bitmapInfoHeader.biHeight;

   bitmapFileHeader.bfOffBits = k+l+ 4*bitmapInfoHeader.biClrUsed;
   bitmapFileHeader.bfSize = k+l+bitmapInfoHeader.biSizeImage;

   n = 0;

#ifdef PRINTD
   cout<< endl << "Printing to the bmp file" << endl;
#endif

   im_out = fopen(image_out_name,"wb");

   if(im_out!=NULL)
   {
      fwrite(&bitmapFileHeader, 1, sizeof(BITMAPFILEHEADER), im_out);
      fwrite(&bitmapInfoHeader, 1, sizeof(BITMAPINFOHEADER), im_out);
      fwrite(biColourPalette, bitmapInfoHeader.biClrUsed*4, 1, im_out);

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

      for(i=(bitmapInfoHeader.biHeight-1); i>=0; i--)
      {
          for(j=0; j<=(bytesperline)-1; j++)
          {
             tk[j] = result[n++];
          }
          fwrite(tk, 1, bytesperline, im_out);
      }

#ifdef PRINTD
      cout << endl << "Writing of  image finished " << endl;
#endif
  }
  else
  {
     cout << endl << "File not opened "<< endl;
  }

 fclose(im_out);
 free(tk);

}
