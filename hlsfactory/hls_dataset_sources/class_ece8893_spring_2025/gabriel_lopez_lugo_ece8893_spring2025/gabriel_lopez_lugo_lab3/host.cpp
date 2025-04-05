#include "dcl.h"

using namespace std;

void load_vec_data_t(data_t src[N * M], packed_32_data_t dst[NUM_VECS_DATA_T]) {
    for (int i = 0; i < NUM_VECS_DATA_T; i++) {
        for (int j = 0; j < NUM_PACKED_DATA_T; j++) {
            dst[i][j] = src[i * NUM_PACKED_DATA_T + j];
        }
    }
}

void load_vec_idx(int src[N * M], packed_32_idx dst[NUM_VECS_IDX]) {
    for (int i = 0; i < NUM_VECS_IDX; i++) {
        for (int j = 0; j < NUM_PACKED_IDX; j++) {
            // Int indices casted to short for smaller overhead since max index is 4096 
            dst[i][j] = static_cast<short>(src[i * NUM_PACKED_IDX + j]);
        }
    }
}

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
    packed_32_data_t values_A_packed[NUM_VECS_DATA_T];
    packed_32_idx column_indices_A_packed[NUM_VECS_IDX];

    // Matrix B (CSC format)
    data_t values_B[M * K];
    int row_indices_B[M * K];
    int col_ptr_B[M + 1];
    int nnz_B;
    packed_32_data_t values_B_packed[NUM_VECS_DATA_T];
    packed_32_idx row_indices_B_packed[NUM_VECS_IDX];

    // Output matrix C (Dense)
    data_t C_ref[N][K];
    data_t C_HLS[N][K];
    packed_32_data_t C_HLS_packed[NUM_VECS_DATA_T];

    // Read matrices from files
    char filename_A[50];
    snprintf(filename_A, sizeof(filename_A), "A_matrix_csr_sparsity_%.2f.bin", SPARSITY);
    read_sparse_matrix_csr(filename_A, values_A, column_indices_A, row_ptr_A, &nnz_A);

    char filename_B[50];
    snprintf(filename_B, sizeof(filename_B), "B_matrix_csc_sparsity_%.2f.bin", SPARSITY);
    read_sparse_matrix_csc(filename_B, values_B, row_indices_B, col_ptr_B, &nnz_B);

    char filename_C[50];
    snprintf(filename_C, sizeof(filename_B), "C_matrix_result_sparsity_%.2f.bin", SPARSITY);
    read_dense_matrix(filename_C, C_ref);
    
    // Initialize output matrix C_HLS
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < K; j++) {
            C_HLS_packed[i][j] = {};
        }
    }

    // Load into packed data
    load_vec_data_t(values_A, values_A_packed);
    load_vec_idx(column_indices_A, column_indices_A_packed);
    load_vec_data_t(values_B, values_B_packed);
    load_vec_idx(row_indices_B, row_indices_B_packed);

    // Call HLS kernel to perform SpMM
    sparse_matrix_multiply_HLS(values_A_packed, column_indices_A_packed, row_ptr_A,
                           values_B_packed, row_indices_B_packed, col_ptr_B, C_HLS_packed);


    // Calculate Mean Squared Error (MSE)
    // Modified to use packed data
    float error = 0;
    for (int i = 0; i < NUM_VECS_DATA_T; i++) {
        for (int j = 0; j < NUM_PACKED_DATA_T; j++) {
            int flat_idx = i * NUM_PACKED_DATA_T + j;
            int row = flat_idx / K;
            int col = flat_idx % K;

            error += std::pow(C_HLS_packed[i][j].to_float() - C_ref[row][col].to_float(), 2);
        }
    }
	error = error / (N * K);
	printf("MSE: %.8f\n", error);

    return 0;
}