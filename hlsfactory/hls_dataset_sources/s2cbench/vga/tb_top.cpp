//========================================================================================
// 
// File Name    : tb_top.cpp
// Description  : Testbench
// Release Date : 10/02/2015
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer,Xu Siyuan
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version   Author     Description
//----------------------------------------------------------------------------------------
//10/02/2015       1.1     PolyU     top vga  testbench
//
//=======================================================================================
#include "tb_top.h"


//-----------------------------
// Receive data thread
//----------------------------
void test_top::recv(){

  // Variables declaration
  unsigned int h_sync_out_write=0;
  unsigned int v_sync_out_write=0;
  unsigned int n_blank_out_write=0;
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

  while(true){


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


    wait();
  }
}



