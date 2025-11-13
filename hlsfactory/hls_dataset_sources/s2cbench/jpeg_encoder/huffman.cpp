//========================================================================================
// 
// File Name    : huffman.cpp
// Description  : JPEG Huffman main functionality 
// Release Date : 30/05/2017
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 30/05/2017        1.0        UTD DARClab     Huffman
//=======================================================================================


#include "huffman.h"

void huffman::huffman_main()
{
  sc_int<QUAN_OUT_WIDTH> rl[RLE_BUFF_LEN];
  int rl_len;
  int k;
  last_bit = 0;
  int i = -3;

  wait();

  while(1)
  {
    //shift output buffer(encode_output) by 64 bits
    if(last_bit<=64)
      last_bit = 0;
    else{
      for(k=64;k<last_bit;k++)
        encode_output[k-64] = encode_output[k];
      last_bit = last_bit - 64;
    }

    i++; //wait 8 cycles
    if(i==8){
      i = 0;

      //read inputs
      for(k=0;k<RLE_BUFF_LEN;k++)
        rl[k] = huffman_in[k].read();
      rl_len = rle_length.read();

      // get huffman code
      jpeg_DCcode(rl[0]);
      jpeg_ACcode(rl,rl_len);
      jpeg_addzero();
    }

    //write outputs
    for(k=0;k<64;k++)
      huffman_out[k].write(encode_output[k]);
    last.write(last_bit);

    wait();
  }
}



sc_uint<4> huffman::jpeg_getCat(sc_int<QUAN_OUT_WIDTH> a)  //get category
{
  if(a==0)
    return 0;
  else if((a<=1)&&(a>=-1))
    return 1;
  else if((a<=3)&&(a>=-3))
    return 2;
  else if((a<=7)&&(a>=-7))
    return 3;
  else if((a<=15)&&(a>=-15))
    return 4;
  else if((a<=31)&&(a>=-31))
    return 5;
  else if((a<=63)&&(a>=-63))
    return 6;
  else if((a<=127)&&(a>=-127))
    return 7;
  else if((a<=255)&&(a>=-255))
    return 8;
  else if((a<=511)&&(a>=-511))
    return 9;
  else if((a<=1023)&&(a>=-1023))
    return 10;
  else
    return 11;
}

void huffman::jpeg_DCcode(sc_int<QUAN_OUT_WIDTH> rl0)
{
  sc_uint<12> powertable[12] = {1,2,4,8,16,32,64,128,256,512,1024,2048};
  sc_uint<5> code_len[12] = {3,4,5,5,7,8,10,12,14,16,18,20};
  sc_uint<1> code[12][9] = {{0,1,0},{0,1,1},{1,0,0},{0,0},{1,0,1}, {1,1,0},{1,1,1,0},{1,1,1,1,0},{1,1,1,1,1,0},{1,1,1,1,1,1,0},{1,1,1,1,1,1,1,0},{1,1,1,1,1,1,1,1,0}};

  sc_uint<4> cat = jpeg_getCat(rl0);
  sc_uint<5> dclen = code_len[cat];  //length of the DC code in # of bits

  sc_uint<1> dc_code[20];

  //get first part of DC code in string
  int q;
  for(q=0;q<(dclen-cat);q++)
    dc_code[q] = code[cat][q];
  
  int j;
  int c = rl0;
  //get second part of DC code in string
  if(rl0<0)
    c+= powertable[cat]-1;
  for(j=dclen-1;j>dclen-cat-1;j--)
  {
    if(c%2==1)
      dc_code[j] = 1;
    else
      dc_code[j] = 0;
    c/=2;
  }

  //write to output register
  int k;
  for(k=0;k<dclen;k++){
    encode_output[last_bit+k] = dc_code[k];
  } 

  last_bit += dclen; //add length of DC code   
}

void huffman::jpeg_ACcode(sc_int<QUAN_OUT_WIDTH> rl[64],sc_uint<8> length)
{
  sc_uint<12> powertable[12] = {1,2,4,8,16,32,64,128,256,512,1024,2048};
  sc_uint<5> codeLen[16][11] = {
    #include "AClen.dat"
  };
  sc_uint<1> code[176][16] = { 
    #include "ACcode.dat"
  };
  
  int i,j;

  for(i=1;i<length;i+=2)
  {   
    sc_int<6> num_zero = rl[i];
    sc_uint<4> cat =  jpeg_getCat(rl[i+1]);
    sc_uint<5> aclen = codeLen[num_zero][cat];  //length of AC code in # of bits
     

    sc_uint<1> ac_code [26];
    // get first part of AC code
    int q;
    for(q=0;q<(aclen-cat);q++)
      ac_code[q] = code[num_zero*11+cat][q];


    // get second part of AC code
    int c = rl[i+1];
    if(rl[i+1]<0)
      c += powertable[cat]-1;
    for(j=aclen-1;j>aclen-cat-1;j--)
    {
      if(c%2==1)
        ac_code[j] = 1;
      else
        ac_code[j] = 0;
      c/=2;
    }

    //write to output register
    for(j=0;j<aclen;j++)
      encode_output[last_bit+j] = ac_code[j];

    last_bit += aclen; //add length of AC code 

  }

} 

void huffman::jpeg_addzero(){
  int m,k,i;

  for(k=0;k<last_bit;k++){
    check_byte[(check_index+k)%8] = encode_output[k];

    if((check_index+k)%8==7){
      m = check_byte[0]&check_byte[1]&check_byte[2]&check_byte[3]&check_byte[4]&check_byte[5]&check_byte[6]&check_byte[7];
      
      if(m==1){
        for(i=last_bit-1;i>k;i--)
          encode_output[i+8] = encode_output[i];
        for(i=0;i<8;i++)
          encode_output[k+1+i] = 0;
        k += 8;
        last_bit +=8;
      }
    }
  }
  check_index = (check_index+last_bit)%8;
}

