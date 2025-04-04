#include "dcl.h"

    hls::stream<int, N+1> row_ptr_A_buf;
    hls::stream<int, N*M> column_indices_A_buf;
    hls::stream<data_t, N*M> values_A_buf;

void load_A_buf (hls::vector<data_t, dk> values_A[N*M],  hls::stream<int, N+1>& row_ptr_A_buf,  
                hls::stream<int, N*M>& column_indices_A_buf, 
                hls::stream<data_t, N*M>& values_A_buf) {
    //load the data
    LOAD: for (int b = 0; b < B; ++b) {
        LOAD_ROW: for (int i = 0; i < N; ++i) {
            // #pragma HLS pipeline II=2
            #pragma HLS performance target_tl=5000
            // hls::vector<fixed_t, dk> Q_row;
            // FETCH_Q: for (int j = 0; j < dk; ++j) {
            //     Q_row[j] = Q[b][i][j];
            // }
            // Q_buffer.write(Q_row);
            Q_buffer.write(Q[b][i]);
        }
    }
}

// Sparse Matrix Multiplication: A (CSR) * B (CSC) = C (Dense)
void sparse_matrix_multiply_HLS(data_t values_A[N * M], int column_indices_A[N * M], int row_ptr_A[N + 1], 
                             data_t values_B[M * K], int row_indices_B[M * K], int col_ptr_B[M + 1], data_t C[N][K]) 
{
#pragma HLS interface m_axi port=values_A offset=slave bundle=mem1
#pragma HLS interface m_axi port=column_indices_A offset=slave bundle=mem1
#pragma HLS interface m_axi port=row_ptr_A offset=slave bundle=mem1

#pragma HLS interface m_axi port=values_B offset=slave bundle=mem2
#pragma HLS interface m_axi port=row_indices_B offset=slave bundle=mem2
#pragma HLS interface m_axi port=col_ptr_B offset=slave bundle=mem2

#pragma HLS interface m_axi port=C offset=slave bundle=mem3

#pragma HLS interface s_axilite port=return

#pragma HLS DATAFLOW

    // #pragma HLS array_partition variable=row_ptr_A dim=1 complete
    // #pragma HLS array_partition variable=column_indices_A dim=1 complete
    // #pragma HLS array_partition variable=values_A dim=1 complete

    // #pragma HLS array_partition variable=col_ptr_B dim=1 complete
    // #pragma HLS array_partition variable=row_indices_B dim=1 complete
    // #pragma HLS array_partition variable=values_B dim=1 complete

    #pragma HLS array_partition variable=C dim=1 complete

    // Buf A
    hls::stream<int, N+1> row_ptr_A_buf;
    hls::stream<int, N*M> column_indices_A_buf;
    hls::stream<data_t, N*M> values_A_buf;

    // // Buf B
    // hls::stream<hls::vector<int, M+1>, 2> col_ptr_B_buf;
    // hls::stream<hls::vector<int, M*K>, 2> row_indices_B_buf;
    // hls::stream<hls::vector<data_t, M*K>, 2> values_B_buf;

    // Buf C
    hls::stream<hls::vector<data_t, N>, 2> C_buf;


    load_A_buf(a)

    //load_buffer(a)

    // Perform Sparse x Sparse Multiplication
    for (int i = 0; i < N; i++) {
        for (int idx_A = row_ptr_A[i]; idx_A < row_ptr_A[i + 1]; idx_A++) {
            // read.buf_a
            int k = column_indices_A[idx_A]; // Column index of A
            data_t value_A = values_A[idx_A];

            // Iterate over columns of B corresponding to row k
            // #pragma HLS pipeline
            for (int idx_B = col_ptr_B[k]; idx_B < col_ptr_B[k + 1]; idx_B++) {
            // #pragma HLS unroll
                int j = row_indices_B[idx_B]; // Column index of B
                data_t value_B = values_B[idx_B];

                // Accumulate the product into C[i][j]
                C[i][j] += value_A * value_B;
            }
        }
    }

    // int stride = 16;

    // // Perform Sparse x Sparse Multiplication
    // for (int i = 0; i < N; i += stride) {
    // #pragma HLS pipeline
    //     for (int ii = i; ii < i+stride; ii++) {
    //     #pragma HLS unroll
    //         for (int idx_A = row_ptr_A[ii]; idx_A < row_ptr_A[ii + 1]; idx_A++) {
    //             int k = column_indices_A[idx_A]; // Column index of A
    //             data_t value_A = values_A[idx_A];

    //             // Iterate over columns of B corresponding to row k
    //             // #pragma HLS pipeline
    //             for (int idx_B = col_ptr_B[k]; idx_B < col_ptr_B[k + 1]; idx_B++) {
    //                 int j = row_indices_B[idx_B]; // Column index of B
    //                 data_t value_B = values_B[idx_B];

    //                 // Accumulate the product into C[i][j]
    //                 C[ii][j] += value_A * value_B;
    //             }
    //         }
    //     }

    // }
}

