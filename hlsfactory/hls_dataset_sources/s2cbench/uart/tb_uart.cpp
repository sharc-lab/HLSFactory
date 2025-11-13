
//========================================================================================
// 
// File Name    : tb_uart.cpp
// Description  : Testbench
// Release Date : 02/12/2014
// Author       : PolyU DARClab
//                Benjamin Carrion Schafer, Nandeesh Veeranna
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
// 02/12/02/2014      1.0       PolyU    UART testbench
//=======================================================================================




#include <systemc.h>
#include "tb_uart.h"
#include "define.h"

/*******************************************
          Transmit Data Thread
*******************************************/


void tb_uart::uart_transmit()
{
    int i = 0;
    int k =0;
    int l =0;
    sc_uint<1> data;
    sc_uint<8> out_data_read;
    unsigned int in_data_read;
    unsigned int receive_data;

    transmit_file = fopen(TRANSMITFILENAME,"rt");

    if(!transmit_file)
    {
        cout << "Could not open " << TRANSMITFILENAME << "n";
        sc_stop();
        exit (-1);
    }

    t_ack.write(0);

    wait();

    while(true)
    {
        while(fscanf(transmit_file,"%u", &in_data_read) != EOF)
        {
            t_data.write(in_data_read);
          
            t_ack.write(1);
            wait();
            t_ack.write(0);

            for(l=0;l<10;l++)
            {
                wait();
            }
        }

        wait();
        wait();

        t_ack.write(0);

        wait();
        wait();
        wait();

        fclose(transmit_file);
        cout << endl << "Compare DATA" << endl;
        compare_data_receive();
        sc_stop();
	wait();

    }
}

/****************************************
          Loop back receive thread
*****************************************/

void tb_uart::uart_receive()
{
    int k = 0;
    sc_uint<1> data;
    //sc_uint<8> out_data_read;
    sc_uint<1> loop_data;
    unsigned int out_data_read;
    receive_file = fopen(RECEIVEFILENAME,"w");

    if(!receive_file)
    {
        cout << "Could not open " << RECEIVEFILENAME << "\n";
        sc_stop();
        exit (-1);
    }

    wait();
    wait();
    wait();

    while(true)
    {
        for(k=0;k<10;k++)
        {
            loop_data=tx.read();
            rx.write(loop_data);
            wait();
        }
        wait();
        out_data_read = r_data.read();
        fprintf(receive_file,"%u \n",out_data_read);
    }
}

void tb_uart::compare_data_receive()
{
    unsigned int data_golden,receive_data,line=1,errors = 0;

    fclose(receive_file);
    receive_file = fopen(RECEIVEFILENAME,"rt");

    if(!receive_file)
    {
        cout << "Could not open " << RECEIVEFILENAME << "\n";
        sc_stop();
        exit (-1);
    }

    transmit_file = fopen(TRANSMITFILENAME,"rt");

    if(!transmit_file)
    {
        cout << "Could not open " << TRANSMITFILENAME << "\n";
        sc_stop();
        exit (-1);
    }

    diff_file = fopen (DIFFERENCEFILENAME, "w");

    if(!diff_file)
    {
        cout << "Could not open " <<DIFFERENCEFILENAME<< "\n";
        sc_stop();
        exit (-1);
    }

    while(fscanf(transmit_file, "%u", &data_golden) != EOF)
    {
        fscanf(receive_file,"%u", &receive_data);
        cout << endl <<"Cycle["<< line << "]: " << data_golden << "-- "<< receive_data;
        if(receive_data != data_golden)
        {
            cout << "\nOutput missmatch [line:" << line << "] Golden:" << data_golden << " -- Output:" << receive_data;
            fprintf(diff_file,"\nOutput missmatch[line:%d] Golden: %d -- Output: %d",line, data_golden, receive_data);
            errors++;
        }
        line ++;
    }

    if(errors == 0)
    cout << endl << "Finished simulation SUCCESSFULLY" << endl;
    else
    cout << endl << "Finished simulation " << errors << " MISSMATCHES between Golden and Simulation" << endl;
    fclose(receive_file);
    fclose(diff_file);
    fclose(transmit_file);
}




