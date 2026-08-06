//Include header files
#include "dcl.h"
#include <ap_int.h>
#include <hls_stream.h>
#include <string.h>
#include "ap_axi_sdata.h"
#include "stdio.h"

//Function to read data values into A, B streams from MatA and MatB tiles (Initialization)
void read_inputAB(int A[][TILE], int B[][TILE], hls::stream<int>& a_pipes0, hls::stream<int>& a_pipes1, hls::stream<int>& a_pipes2,
    hls::stream<int>& a_pipes3, hls::stream<int>& a_pipes4, hls::stream<int>& a_pipes5, hls::stream<int>& a_pipes6,
    hls::stream<int>& a_pipes7, hls::stream<int>& a_pipes8, hls::stream<int>& a_pipes9, hls::stream<int>& a_pipes10,
    hls::stream<int>& a_pipes11, hls::stream<int>& a_pipes12, hls::stream<int>& a_pipes13, hls::stream<int>& a_pipes14,
    hls::stream<int>& a_pipes15,
    hls::stream<int>& b_pipes0, hls::stream<int>& b_pipes1, hls::stream<int>& b_pipes2,
    hls::stream<int>& b_pipes3, hls::stream<int>& b_pipes4, hls::stream<int>& b_pipes5, hls::stream<int>& b_pipes6,
    hls::stream<int>& b_pipes7, hls::stream<int>& b_pipes8, hls::stream<int>& b_pipes9, hls::stream<int>& b_pipes10,
    hls::stream<int>& b_pipes11, hls::stream<int>& b_pipes12, hls::stream<int>& b_pipes13, hls::stream<int>& b_pipes14,
    hls::stream<int>& b_pipes15)
{
ReadMemory_AB:
    int valueA, valueB;
    for (int i = 0; i < TILE; i++)
    {
        for (int j = 0; j < TILE; j++)
        {
            valueA = A[i][j];
            if (i == 0)
            {
                a_pipes0.write(valueA);
            }
            if (i == 1)
            {
                a_pipes1.write(valueA);
            }
            if (i == 2)
            {
                a_pipes2.write(valueA);
            }
            if (i == 3)
            {
                a_pipes3.write(valueA);
            }
            if (i == 4)
            {
                a_pipes4.write(valueA);
            }
            if (i == 5)
            {
                a_pipes5.write(valueA);
            }
            if (i == 6)
            {
                a_pipes6.write(valueA);
            }
            if (i == 7)
            {
                a_pipes7.write(valueA);
            }
            if (i == 8)
            {
                a_pipes8.write(valueA);
            }
            if (i == 9)
            {
                a_pipes9.write(valueA);
            }
            if (i == 10)
            {
                a_pipes10.write(valueA);
            }
            if (i == 11)
            {
                a_pipes11.write(valueA);
            }
            if (i == 12)
            {
                a_pipes12.write(valueA);
            }
            if (i == 13)
            {
                a_pipes13.write(valueA);
            }
            if (i == 14)
            {
                a_pipes14.write(valueA);
            }
            if (i == 15)
            {
                a_pipes15.write(valueA);
            }
            valueB = B[i][j];
            if (j == 0)
            {
                b_pipes0.write(valueB);
            }
            if (j == 1)
            {
                b_pipes1.write(valueB);
            }
            if (j == 2)
            {
                b_pipes2.write(valueB);
            }
            if (j == 3)
            {
                b_pipes3.write(valueB);
            }
            if (j == 4)
            {
                b_pipes4.write(valueB);
            }
            if (j == 5)
            {
                b_pipes5.write(valueB);
            }
            if (j == 6)
            {
                b_pipes6.write(valueB);
            }
            if (j == 7)
            {
                b_pipes7.write(valueB);
            }
            if (j == 8)
            {
                b_pipes8.write(valueB);
            }
            if (j == 9)
            {
                b_pipes9.write(valueB);
            }
            if (j == 10)
            {
                b_pipes10.write(valueB);
            }
            if (j == 11)
            {
                b_pipes11.write(valueB);
            }
            if (j == 12)
            {
                b_pipes12.write(valueB);
            }
            if (j == 13)
            {
                b_pipes13.write(valueB);
            }
            if (j == 14)
            {
                b_pipes14.write(valueB);
            }
            if (j == 15)
            {
                b_pipes15.write(valueB);
            }
        }
    }
}

//Function to write zeros to the C streams (Initialization)
void read_inputC(hls::stream<int>& c_pipes0, hls::stream<int>& c_pipes1, hls::stream<int>& c_pipes2, hls::stream<int>& c_pipes3,
    hls::stream<int>& c_pipes4, hls::stream<int>& c_pipes5, hls::stream<int>& c_pipes6, hls::stream<int>& c_pipes7,
    hls::stream<int>& c_pipes8, hls::stream<int>& c_pipes9, hls::stream<int>& c_pipes10, hls::stream<int>& c_pipes11,
    hls::stream<int>& c_pipes12, hls::stream<int>& c_pipes13, hls::stream<int>& c_pipes14, hls::stream<int>& c_pipes15)
{
WriteCInitial:
    c_pipes0.write(0);
    c_pipes1.write(0);
    c_pipes2.write(0);
    c_pipes3.write(0);
    c_pipes4.write(0);
    c_pipes5.write(0);
    c_pipes6.write(0);
    c_pipes7.write(0);
    c_pipes8.write(0);
    c_pipes9.write(0);
    c_pipes10.write(0);
    c_pipes11.write(0);
    c_pipes12.write(0);
    c_pipes13.write(0);
    c_pipes14.write(0);
    c_pipes15.write(0);
}

//Function to write the result from the C streams to MatC, Note that streams A and B are passed as dummy arguments because HLS mandates that every stream is produced and consumed once.
void write_resultC(int C[][TILE], hls::stream<int>& c_pipes0, hls::stream<int>& c_pipes1, hls::stream<int>& c_pipes2,
    hls::stream<int>& c_pipes3, hls::stream<int>& c_pipes4, hls::stream<int>& c_pipes5, hls::stream<int>& c_pipes6,
    hls::stream<int>& c_pipes7, hls::stream<int>& c_pipes8, hls::stream<int>& c_pipes9, hls::stream<int>& c_pipes10,
    hls::stream<int>& c_pipes11, hls::stream<int>& c_pipes12, hls::stream<int>& c_pipes13, hls::stream<int>& c_pipes14,
    hls::stream<int>& c_pipes15,
    hls::stream<int>& pipe1, hls::stream<int>& pipe2, hls::stream<int>& pipe3, hls::stream<int>& pipe4, hls::stream<int>& pipe5,
    hls::stream<int>& pipe6, hls::stream<int>& pipe7, hls::stream<int>& pipe8, hls::stream<int>& pipe9, hls::stream<int>& pipe10,
    hls::stream<int>& pipe11, hls::stream<int>& pipe12, hls::stream<int>& pipe13,
    hls::stream<int>& pipe14, hls::stream<int>& pipe15, hls::stream<int>& pipe16,
    hls::stream<int>& pipe17, hls::stream<int>& pipe18, hls::stream<int>& pipe19,
    hls::stream<int>& pipe20, hls::stream<int>& pipe21, hls::stream<int>& pipe22,
    hls::stream<int>& pipe23, hls::stream<int>& pipe24, hls::stream<int>& pipe25,
    hls::stream<int>& pipe26, hls::stream<int>& pipe27, hls::stream<int>& pipe28,
    hls::stream<int>& pipe29, hls::stream<int>& pipe30, hls::stream<int>& pipe31,
    hls::stream<int>& pipe32)
{
MemoryWrite:
    for (int row = 0; row < TILE; row++)
    {
        for (int j = 0; j < TILE; j++)
        {
            if (row == 0)
            {
                C[row][j] = c_pipes0.read();
            }
            if (row == 1)
            {
                C[row][j] = c_pipes1.read();
            }
            if (row == 2)
            {
                C[row][j] = c_pipes2.read();
            }
            if (row == 3)
            {
                C[row][j] = c_pipes3.read();
            }
            if (row == 4)
            {
                C[row][j] = c_pipes4.read();
            }
            if (row == 5)
            {
                C[row][j] = c_pipes5.read();
            }
            if (row == 6)
            {
                C[row][j] = c_pipes6.read();
            }
            if (row == 7)
            {
                C[row][j] = c_pipes7.read();
            }
            if (row == 8)
            {
                C[row][j] = c_pipes8.read();
            }
            if (row == 9)
            {
                C[row][j] = c_pipes9.read();
            }
            if (row == 10)
            {
                C[row][j] = c_pipes10.read();
            }
            if (row == 11)
            {
                C[row][j] = c_pipes11.read();
            }
            if (row == 12)
            {
                C[row][j] = c_pipes12.read();
            }
            if (row == 13)
            {
                C[row][j] = c_pipes13.read();
            }
            if (row == 14)
            {
                C[row][j] = c_pipes14.read();
            }
            if (row == 15)
            {
                C[row][j] = c_pipes15.read();
            }
        }
    }
} 


// Core function of processing element - The input values are read from Streams A & B. Computes A.B and accumulates it with the previous C value
void ProcessingElement(hls::stream<int>& a_in, hls::stream<int>& a_out, hls::stream<int>& b_in, hls::stream<int>& b_out,
    hls::stream<int>& c_in, hls::stream<int>& c_out, int pe_row, int pe_col)
{
PE_Compute:
    int a_buffer;
    int b_buffer;
    int c_buffer = 0;
    int count = 0;
    int c_prev;

    //If its a PE in the left column, drain the zero value
    if (pe_col == 0)
    {
        c_prev = c_in.read();
    }
    
    //pe_row + pe_col + TILE represents the number of cycles the PE will be active
    for (int k = 0; k < pe_row + pe_col + TILE; k++)
    {
        #pragma HLS PIPELINE II = 1
        // Read a_buffer value from a_pipe_in
        //Since we are not appending zeros while feeding the inputs, each PE has to wait for a specific number of iterations before reading the input values
        if (pe_col == 0)
        {
            if (a_in.empty())
            {
                a_buffer = 0;
            }
            else if (count < pe_row)
            {
                a_buffer = 0;
                a_out.write(a_buffer);
            }
            else
            {
                a_buffer = a_in.read();
                a_out.write(a_buffer);
            }
        }
        else if (a_in.empty() || count < pe_col)
        {
            a_buffer = 0;
        }
        else
        {
            a_buffer = a_in.read();
            if (pe_col + 1 < TILE)
            {
                a_out.write(a_buffer);
            }
        }
        // Read b_buffer value from b_pipe_in
        //Since we are not appending zeros while feeding the inputs, each PE has to wait for a specific number of iterations before reading the input values
        if (pe_row == 0)
        {
            if (b_in.empty())
            {
                b_buffer = 0;
            }
            else if (count < pe_col)
            {
                b_buffer = 0;
                b_out.write(b_buffer);
            }
            else
            {
                b_buffer = b_in.read();
                b_out.write(b_buffer);
            }
        }
        else if (b_in.empty() || count < pe_row)
        {
            b_buffer = 0;
        }
        else
        {
            b_buffer = b_in.read();
            if (pe_row + 1 < TILE)
            {
                b_out.write(b_buffer);
            }
        }
        c_buffer += a_buffer * b_buffer;
        count++;

        //Pass the C values from c_in stream
        if (!c_in.empty())
        {
            c_out.write(c_in.read());
        }
    }
    c_out.write(c_buffer); //Write the new C value from this PE to c_out
} 

//Core systolic array function
void systolic_array(int A[][TILE], int B[][TILE], int C[][TILE])
{
    // Define the data flow region
    #pragma HLS dataflow
    #pragma HLS interface mode = ap_ctrl_chain port = return

    // Array of stream declaration
    hls::stream<int> a_pipes[TILE][TILE + 1]; // For passing MatA values
    hls::stream<int> b_pipes[TILE + 1][TILE]; // For passing MatB values
    hls::stream<int> c_pipes[TILE][TILE + 1]; // For passing MatC (Product) values

    // Define the size of the stream - If not defined, HLS takes the default value as two
    #pragma HLS STREAM variable = a_pipes depth = 10
    #pragma HLS STREAM variable = b_pipes depth = 10
    #pragma HLS STREAM variable = c_pipes depth = 10

    // Populate initial values in the streams
     //--------------------------------------
     read_inputAB(A, B, a_pipes[0][0], a_pipes[1][0], a_pipes[2][0], a_pipes[3][0], a_pipes[4][0], a_pipes[5][0], a_pipes[6][0], a_pipes[7][0],
            a_pipes[8][0], a_pipes[9][0], a_pipes[10][0], a_pipes[11][0], a_pipes[12][0], a_pipes[13][0], a_pipes[14][0], a_pipes[15][0],
            b_pipes[0][0], b_pipes[0][1], b_pipes[0][2], b_pipes[0][3], b_pipes[0][4], b_pipes[0][5], b_pipes[0][6], b_pipes[0][7],
            b_pipes[0][8], b_pipes[0][9], b_pipes[0][10], b_pipes[0][11], b_pipes[0][12], b_pipes[0][13], b_pipes[0][14], b_pipes[0][15]);
     read_inputC(c_pipes[0][0], c_pipes[1][0], c_pipes[2][0], c_pipes[3][0], c_pipes[4][0], c_pipes[5][0], c_pipes[6][0], c_pipes[7][0],
            c_pipes[8][0], c_pipes[9][0], c_pipes[10][0], c_pipes[11][0], c_pipes[12][0], c_pipes[13][0], c_pipes[14][0], c_pipes[15][0]);

     // Main Compute Loop
     //------------------
    for (int i = 0; i < TILE; i++)
    {
    #pragma HLS UNROLL
        for (int j = 0; j < TILE; j++)
        {
            #pragma HLS UNROLL
            ProcessingElement(a_pipes[i][j], a_pipes[i][j + 1], b_pipes[i][j], b_pipes[i + 1][j], c_pipes[i][j], c_pipes[i][j + 1], i, j);
        }
    }

    // Write result back to memory
    write_resultC(C, c_pipes[0][16], c_pipes[1][16], c_pipes[2][16], c_pipes[3][16], c_pipes[4][16], c_pipes[5][16], c_pipes[6][16], c_pipes[7][16],
        c_pipes[8][16], c_pipes[9][16], c_pipes[10][16], c_pipes[11][16], c_pipes[12][16], c_pipes[13][16], c_pipes[14][16], c_pipes[15][16],
        a_pipes[0][16], a_pipes[1][16], a_pipes[2][16], a_pipes[3][16], a_pipes[4][16], a_pipes[5][16], a_pipes[6][16], a_pipes[7][16],
        a_pipes[8][16], a_pipes[9][16], a_pipes[10][16], a_pipes[11][16], a_pipes[12][16], a_pipes[13][16], a_pipes[14][16], a_pipes[15][16],
        b_pipes[16][0], b_pipes[16][1], b_pipes[16][2], b_pipes[16][3], b_pipes[16][4], b_pipes[16][5], b_pipes[16][6], b_pipes[16][7],
        b_pipes[16][8], b_pipes[16][9], b_pipes[16][10], b_pipes[16][11], b_pipes[16][12], b_pipes[16][13], b_pipes[16][14], b_pipes[16][15]);
}

//Function read MatA and MatB tile values from DRAM
void ReadInputs(int MatA_DRAM[][N], int MatB_DRAM[][K], int MatA[][TILE], int MatB[][TILE], int m0, int n0, int k0)
{
    #pragma HLS pipeline off
    for (int i = 0; i < TILE; i++)
    {
        #pragma HLS LOOP_TRIPCOUNT min = 16 max = 16
        for (int j = 0; j < TILE; j++)
        {
         #pragma HLS PIPELINE
         #pragma HLS LOOP_TRIPCOUNT min = 16 max = 16
            MatA[i][j] = MatA_DRAM[i + m0][j + n0];
            MatB[i][j] = MatB_DRAM[i + n0][j + k0];
        }
    }
}

//Function to intialize the MatC values to 0 (Initialization)
void InitC(int MatC[][TILE])
{
    for (int i = 0; i < TILE; i++)
    {
        for (int j = 0; j < TILE; j++)
        {
            MatC[i][j] = 0;
        }
    }
}

//Function to write the MatC product tile to DRAM
void writeC(int MatC[][TILE], int MatC_DRAM[][K], int m0, int k0)
{
    #pragma HLS pipeline off
    for (int i = 0; i < TILE; i++)
    {
        #pragma HLS LOOP_TRIPCOUNT min = 16 max = 16
        for (int j = 0; j < TILE; j++)
        {
            #pragma HLS LOOP_TRIPCOUNT min = 16 max = 16
            #pragma HLS PIPELINE
            MatC_DRAM[i + m0][j + k0] = MatC[i][j];
            MatC[i][j] = 0;
        }
    }
}

//Function to accumulate add the C values from different tiles and store it in MatC
void addC(int C[][TILE], int C0[][TILE], int C1[][TILE])
{
    for (int i = 0; i < TILE; i++)
    {
        #pragma HLS unroll
        for (int j = 0; j < TILE; j++)
        {
            #pragma HLS unroll
            C[i][j] += C0[i][j] + C1[i][j];
        }
    }
}

//Main function - Matrix Multiplication
void matrix_mul(int MatA_DRAM[M][N], int MatB_DRAM[N][K], int MatC_DRAM[M][K])
{

    //Define the pragma interfaces for memory access
    #pragma HLS interface m_axi depth = 100000 port = MatA_DRAM offset = slave bundle = memA
    #pragma HLS interface m_axi depth = 100000 port = MatB_DRAM offset = slave bundle = memB
    #pragma HLS interface m_axi depth = 100000 port = MatC_DRAM offset = slave bundle = memC
    #pragma HLS interface s_axilite port = return

    //Define MatA, B and C ping, pong buffers
    int MatA0_pong[TILE][TILE];
    int MatB0_pong[TILE][TILE];
    int MatC0_pong[TILE][TILE];
    int MatA0_ping[TILE][TILE];
    int MatB0_ping[TILE][TILE];
    int MatC0_ping[TILE][TILE];
    int MatA1_ping[TILE][TILE];
    int MatB1_ping[TILE][TILE];
    int MatC1_ping[TILE][TILE];
    int MatA1_pong[TILE][TILE];
    int MatB1_pong[TILE][TILE];
    int MatC1_pong[TILE][TILE];
    int MatC[TILE][TILE];

    //Partitioning the MatC buffers 
    #pragma HLS array_partition variable=MatC dim=1 complete
    #pragma HLS array_partition variable=MatC dim=2 complete
    #pragma HLS array_partition variable=MatC0_ping type= complete
    #pragma HLS array_partition variable=MatC0_pong type= complete
    #pragma HLS array_partition variable=MatC1_ping type= complete
    #pragma HLS array_partition variable=MatC1_pong type= complete

    //Initialie the MatC matrix to zeros
    InitC(MatC);

    //Iterate over the row dimension of MatA
    for (int mt = 0; mt < M; mt = mt + TILE)
    {
        //Iterate over the column dimension of MatB
        for (int kt = 0; kt < K; kt = kt + TILE)
        {
            //Read the first tile values into ping buffers
            ReadInputs(MatA_DRAM, MatB_DRAM, MatA0_ping, MatB0_ping, mt, 0, kt);
            ReadInputs(MatA_DRAM, MatB_DRAM, MatA1_ping, MatB1_ping, mt, TILE, kt);

            //Iterate over the common dimension (Column of MatA and row of MatB)
            for (int nt = 0; nt < N; nt = nt + 2 * TILE)
            {
                if (nt % 2 == 0)
                {
                    //Execute from ping buffer
                    //Execute two systolic arrays in parallel
                    systolic_array(MatA0_ping, MatB0_ping, MatC0_ping);
                    systolic_array(MatA1_ping, MatB1_ping, MatC1_ping);
                    addC(MatC, MatC0_ping, MatC1_ping);
                    //Read to pong buffer
                    if (nt + 3 * TILE < N)
                    {
                        ReadInputs(MatA_DRAM, MatB_DRAM, MatA0_pong, MatB0_pong, mt, nt + 2 * TILE, kt);
                        ReadInputs(MatA_DRAM, MatB_DRAM, MatA1_pong, MatB1_pong, mt, nt + 3 * TILE, kt);
                    }
                }
                else
                {
                    //Execute from pong buffer
                    //Execute two systolic arrays in parallel
                    systolic_array(MatA0_pong, MatB0_pong, MatC0_pong);
                    systolic_array(MatA1_pong, MatB1_pong, MatC1_pong);
                    addC(MatC, MatC0_pong, MatC1_pong);
                    //Read to ping buffer
                    if (nt + 3 * TILE < N)
                    {
                        ReadInputs(MatA_DRAM, MatB_DRAM, MatA0_ping, MatB0_ping, mt, nt + 2 * TILE, kt);
                        ReadInputs(MatA_DRAM, MatB_DRAM, MatA1_ping, MatB1_ping, mt, nt + 3 * TILE, kt);
                    }
                }
            }

            //Write output C tile value to DRAM
            writeC(MatC, MatC_DRAM, mt, kt);
        }
    }
}