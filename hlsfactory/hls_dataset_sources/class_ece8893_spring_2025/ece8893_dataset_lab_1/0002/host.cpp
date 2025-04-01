#include "dcl.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

// Function to read a sparse matrix in CSR format
void read_sparse_matrix_csr(const char *filename, data_t values[], int column_indices[], int row_ptr[], int *nnz) {
    FILE *file = fopen(filename, "rb");
    printf("Reading sparse matrix from file: %s\n", filename);
    if (!file) {
        perror("Failed to open CSR file");
        exit(1);
    }

    fread(nnz, sizeof(int), 1, file);
    fread(values, sizeof(data_t), *nnz, file);
    fread(column_indices, sizeof(int), *nnz, file);
    fread(row_ptr, sizeof(int), N + 1, file);
    fclose(file);
}

// Function to read a sparse matrix in CSC format
void read_sparse_matrix_csc(const char *filename, data_t values[], int row_indices[], int col_ptr[], int *nnz) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open CSC file");
        exit(1);
    }

    fread(nnz, sizeof(int), 1, file);
    fread(values, sizeof(data_t), *nnz, file);
    fread(row_indices, sizeof(int), *nnz, file);
    fread(col_ptr, sizeof(int), M + 1, file);
    fclose(file);
}

// Function to read a dense matrix from a binary file
void read_dense_matrix(const char *filename, data_t C[N][K]) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open dense matrix file");
        exit(1);
    }

    fread(C, sizeof(data_t), N * K, file);
    fclose(file);
}

// Utility to pack array of data_t into wide_t
void pack_data_array(data_t* input, wide_t* output, int size) {
    for (int i = 0; i < size / 32; ++i) {
        wide_t packed = 0;
        for (int j = 0; j < 32; ++j) {
            *((data_t*)&packed + j) = input[i * 32 + j];
        }
        output[i] = packed;
    }
}

int main() {
    // Allocate CSR matrix A
    data_t unpacked_values_A[N * M];
    int column_indices_A[N * M];
    int row_ptr_A[N + 1];
    int nnz_A;

    // Allocate CSC matrix B
    data_t unpacked_values_B[M * K];
    int row_indices_B[M * K];
    int col_ptr_B[M + 1];
    int nnz_B;

    // Output matrices
    data_t C_ref[N][K];   // golden output
    data_t C_HLS[N][K];   // HLS output

    // Allocate wide versions
    wide_t values_A[N * M / 32];
    wide_t values_B[M * K / 32];

    // Read files
    char filename_A[64];
    snprintf(filename_A, sizeof(filename_A), "A_matrix_csr_sparsity_%.2f.bin", SPARSITY);
    read_sparse_matrix_csr("/usr/scratch/akshat/fpga/lab3/A_matrix_csr_sparsity_0.50.bin", unpacked_values_A, column_indices_A, row_ptr_A, &nnz_A);

    char filename_B[64];
    snprintf(filename_B, sizeof(filename_B), "B_matrix_csc_sparsity_%.2f.bin", SPARSITY);
    read_sparse_matrix_csc("/usr/scratch/akshat/fpga/lab3/B_matrix_csc_sparsity_0.50.bin", unpacked_values_B, row_indices_B, col_ptr_B, &nnz_B);

    char filename_C[64];
    snprintf(filename_C, sizeof(filename_C), "C_matrix_result_sparsity_%.2f.bin", SPARSITY);
    read_dense_matrix("/usr/scratch/akshat/fpga/lab3/C_matrix_result_sparsity_0.50.bin", C_ref);

    // Pack A and B into wide_t arrays
    pack_data_array(unpacked_values_A, values_A, N * M);
    pack_data_array(unpacked_values_B, values_B, M * K);

    // Initialize HLS output matrix
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < K; j++) {
            C_HLS[i][j] = 0;
        }
    }

    // Run HLS kernel
    sparse_matrix_multiply_HLS(
        values_A, column_indices_A, row_ptr_A,
        values_B, row_indices_B, col_ptr_B,
        C_HLS
    );

    // Compare output
    float error = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < K; j++) {
            float diff = C_HLS[i][j].to_float() - C_ref[i][j].to_float();
            error += diff * diff;
        }
    }
    error /= (N * K);
    printf("MSE: %.8f\n", error);

    return 0;
}
