#include "dcl.h"

using namespace std;

// Function to read a sparse matrix in CSR format
void read_sparse_matrix_csr(const char *filename, data_t values[], int column_indices[], int row_ptr[], int *nnz) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
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
        perror("Failed to open file");
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
        perror("Failed to open file");
        exit(1);
    }

    fread(C, sizeof(data_t), N * K, file);
}

// Sparse Matrix Multiplication: A (CSR) * B (CSC) = C (Dense)
void sparse_matrix_multiply(data_t values_A[], int column_indices_A[], int row_ptr_A[], int nnz_A,
                             data_t values_B[], int row_indices_B[], int col_ptr_B[], int nnz_B,
                             float C[N][K]) 
{
    // Perform Sparse x Sparse Multiplication
    for (int i = 0; i < N; i++) {
        for (int idx_A = row_ptr_A[i]; idx_A < row_ptr_A[i + 1]; idx_A++) {
            int k = column_indices_A[idx_A]; // Column index of A
            float value_A = values_A[idx_A].to_float();

            // Iterate over columns of B corresponding to row k
            for (int idx_B = col_ptr_B[k]; idx_B < col_ptr_B[k + 1]; idx_B++) {
                int j = row_indices_B[idx_B]; // Column index of B
                float value_B = values_B[idx_B].to_float();

                // Accumulate the product into C[i][j]
                C[i][j] += value_A * value_B;
            }
        }
    }
}

int main() {
    // Matrix A (CSR format)
    data_t values_A[N * M];
    int column_indices_A[N * M];
    int row_ptr_A[N + 1];
    int nnz_A;

    // Read matrix A
    char filename_A[50];
    snprintf(filename_A, sizeof(filename_A), "A_matrix_csr_sparsity_%.2f.bin", SPARSITY);
    read_sparse_matrix_csr(filename_A, values_A, column_indices_A, row_ptr_A, &nnz_A);

    ap_uint<16> column_indices_A_comp[N * M];
    ap_uint<16> row_ptr_A_comp[N + 1];
    for (int i = 0; i < N * M; i++) {
        column_indices_A_comp[i] = column_indices_A[i];
    }
    for (int i = 0; i < N + 1; i++) {
        row_ptr_A_comp[i] = row_ptr_A[i];
    }

    mem_t values_A_mem[MPDA];
    mem_t column_indices_A_mem[MPDA];

    for (int i = 0; i < MPDA; i++) {
        for (int j = 0; j < DPM; j++) {
            values_A_mem[i].range(15 + (j * 16), 0 + (j * 16)) = values_A[i * DPM + j].range(15, 0);
            column_indices_A_mem[i].range(15 + (j * 16), 0 + (j * 16)) = column_indices_A_comp[i * DPM + j].range(15, 0);
        }
    }

    // Matrix B (CSC format)
    data_t values_B[M * K];
    int row_indices_B[M * K];
    int col_ptr_B[M + 1];
    int nnz_B;

    // Read matrix B
    char filename_B[50];
    snprintf(filename_B, sizeof(filename_B), "B_matrix_csc_sparsity_%.2f.bin", SPARSITY);
    read_sparse_matrix_csc(filename_B, values_B, row_indices_B, col_ptr_B, &nnz_B);

    ap_uint<16> row_indices_B_comp[M * K];
    ap_uint<16> col_ptr_B_comp[M + 1];
    for (int i = 0; i < M * K; i++) {
        row_indices_B_comp[i] = row_indices_B[i];
    }
    for (int i = 0; i < M + 1; i++) {
        col_ptr_B_comp[i] = col_ptr_B[i];
    }

    mem_t values_B_mem[MPDB];
    mem_t row_indices_B_mem[MPDB];

    for (int i = 0; i < MPDB; i++) {
        for (int j = 0; j < DPM; j++) {
            values_B_mem[i].range(15 + (j * 16), 0 + (j * 16)) = values_B[i * DPM + j].range(15, 0);
            row_indices_B_mem[i].range(15 + (j * 16), 0 + (j * 16)) = row_indices_B_comp[i * DPM + j].range(15, 0);
        }
    }

    // Output matrix C (Dense)
    data_t C_ref[N][K];
    data_t C_HLS[N][K];
    mem_t C_HLS_mem[N][K/DPM];

    char filename_C[50];
    snprintf(filename_C, sizeof(filename_B), "C_matrix_result_sparsity_%.2f.bin", SPARSITY);
    read_dense_matrix(filename_C, C_ref);
    
    // Initialize output matrix C_HLS
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < K/DPM; j++) {
            C_HLS_mem[i][j] = 0;
        }
    }

    // Call HLS kernel to perform SpMM
    sparse_matrix_multiply_HLS(values_A_mem, column_indices_A_mem, row_ptr_A_comp,
                           values_B_mem, row_indices_B_mem, col_ptr_B_comp, C_HLS_mem);

    for (int i = 0; i < N; i++) {
        for (int k = 0; k < K/DPM; k++) {
            mem_t data = C_HLS_mem[i][k];
            for (int kk = 0; kk < 32; kk++) {
                C_HLS[i][k * 32 + kk].range(15, 0) = data.range(15 + (kk * 16), 0 + (kk * 16));
            }
        }
    }

 	float error = 0;
	// compare HLS output and reference output tensor
	for(int i = 0; i < N; i++) {
		for(int j = 0; j < K; j++) {
            printf("Reference: %f | Computed: %f\n", C_ref[i][j].to_float(), C_HLS[i][j].to_float());
			error += std::pow(C_HLS[i][j].to_float() - C_ref[i][j].to_float(), 2);
		}
	}
	error = error / (N * K);
	printf("MSE: %.8f\n", error);

    return 0;
}