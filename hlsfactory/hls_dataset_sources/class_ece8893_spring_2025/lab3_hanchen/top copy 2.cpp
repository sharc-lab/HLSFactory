#include "dcl.h"
#include <hls_stream.h>

#define MAX_NNZ_PER_COL 64

struct NonZeroA {
    int row;
    int col;
    data_t val;
    bool last;
};

struct NonZeroC {
    int row;
    int col;
    data_t val;
    bool last;
};

// Load Matrix B into local memory
static void fetchMatrixB(
    data_t* B_values,
    int* B_row_indices,
    int* B_col_ptr,
    data_t localB[M][MAX_NNZ_PER_COL],
    int localRowIdx[M][MAX_NNZ_PER_COL],
    int localLen[M]
) {
    int colPointers[M+1];

    for (int i = 0; i <= M; i++) {
    #pragma HLS PIPELINE II=1
        colPointers[i] = B_col_ptr[i];
    }

    for (int i = 0; i < M; i++) {
        localLen[i] = colPointers[i+1] - colPointers[i];
        for (int j = 0; j < localLen[i]; j++) {
        #pragma HLS PIPELINE II=1
            localB[i][j] = B_values[colPointers[i] + j];
            localRowIdx[i][j] = B_row_indices[colPointers[i] + j];
        }
    }
}

// Stream non-zeros of Matrix A
static void streamMatrixA(
    data_t* A_values,
    int* A_col_indices,
    int* A_row_ptr,
    hls::stream<NonZeroA>& AStream
) {
    int rowPointers[N+1];

    for (int i = 0; i <= N; i++) {
    #pragma HLS PIPELINE II=1
        rowPointers[i] = A_row_ptr[i];
    }

    for (int i = 0; i < N; i++) {
        for (int idx = rowPointers[i]; idx < rowPointers[i+1]; idx++) {
            NonZeroA pkt = {i, A_col_indices[idx], A_values[idx], false};
            AStream.write(pkt);
        }
        AStream.write({i, -1, 0, true});
    }
}

// Perform Matrix Multiplication
static void matrixCompute(
    hls::stream<NonZeroA>& AStream,
    const data_t localB[M][MAX_NNZ_PER_COL],
    const int localRowIdx[M][MAX_NNZ_PER_COL],
    const int localLen[M],
    hls::stream<NonZeroC>& CStream
) {
    static data_t accum[K];
    #pragma HLS bind_storage variable=accum type=ram_2p impl=bram

    for (int i = 0; i < N; i++) {
        #pragma HLS PIPELINE II=1
        for (int j = 0; j < K; j++) {
            accum[j] = 0;
        }

        while (true) {
            NonZeroA pkt = AStream.read();
            if (pkt.last) break;

            int col = pkt.col;
            data_t aval = pkt.val;
            
            for (int k = 0; k < localLen[col]; k++) {
                int rowB = localRowIdx[col][k];
                data_t bval = localB[col][k];
                accum[rowB] += aval * bval;
            }
        }

        for (int j = 0; j < K; j++) {
            CStream.write({i, j, accum[j], (j == K-1)});
        }
    }
}

// Store result into Matrix C
static void storeMatrixC(
    hls::stream<NonZeroC>& CStream,
    data_t C[N][K]
) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < K; j++) {
            NonZeroC pkt = CStream.read();
            C[pkt.row][pkt.col] = pkt.val;
        }
    }
}

// Main Function
void sparse_matrix_multiply_HLS(
    data_t A_values[N*M],
    int A_col_indices[N*M],
    int A_row_ptr[N+1],
    data_t B_values[M*K],
    int B_row_indices[M*K],
    int B_col_ptr[M+1],
    data_t C[N][K]
) {
    #pragma HLS INTERFACE m_axi port=A_values offset=slave bundle=mem1
    #pragma HLS INTERFACE m_axi port=A_col_indices offset=slave bundle=mem1
    #pragma HLS INTERFACE m_axi port=A_row_ptr offset=slave bundle=mem1

    #pragma HLS INTERFACE m_axi port=B_values offset=slave bundle=mem2
    #pragma HLS INTERFACE m_axi port=B_row_indices offset=slave bundle=mem2
    #pragma HLS INTERFACE m_axi port=B_col_ptr offset=slave bundle=mem2

    #pragma HLS INTERFACE m_axi port=C offset=slave bundle=mem3
    #pragma HLS INTERFACE s_axilite port=return

    static data_t localB[M][MAX_NNZ_PER_COL];
    static int localRowIdx[M][MAX_NNZ_PER_COL];
    static int localLen[M];

    #pragma HLS bind_storage variable=localB type=ram_1p impl=bram
    #pragma HLS bind_storage variable=localRowIdx type=ram_1p impl=bram

    fetchMatrixB(B_values, B_row_indices, B_col_ptr, localB, localRowIdx, localLen);

    static hls::stream<NonZeroA> AStream("AStream");
    static hls::stream<NonZeroC> CStream("CStream");

    #pragma HLS STREAM variable=AStream depth=64
    #pragma HLS STREAM variable=CStream depth=64

    #pragma HLS DATAFLOW
    streamMatrixA(A_values, A_col_indices, A_row_ptr, AStream);
    matrixCompute(AStream, localB, localRowIdx, localLen, CStream);
    storeMatrixC(CStream, C);
}
