#include "dcl.h"

using namespace std;

void read_sparse_matrix_csr(const char *filename, data_t values[], int column_indices[], int row_ptr[], int *nnz) {
    FILE *file = fopen(filename, "rb");
    fread(nnz, sizeof(int), 1, file);
    fread(values, sizeof(data_t), *nnz, file);
    fread(column_indices, sizeof(int), *nnz, file);
    fread(row_ptr, sizeof(int), N + 1, file);
    fclose(file);
}

void read_sparse_matrix_csc(const char *filename, data_t values[], int row_indices[], int col_ptr[], int *nnz) {
    FILE *file = fopen(filename, "rb");
    fread(nnz, sizeof(int), 1, file);
    fread(values, sizeof(data_t), *nnz, file);
    fread(row_indices, sizeof(int), *nnz, file);
    fread(col_ptr, sizeof(int), M + 1, file);
    fclose(file);
}

void read_dense_matrix(const char *filename, data_t C[N][K]) {
    FILE *file = fopen(filename, "rb");
    fread(C, sizeof(data_t), N * K, file);
    fclose(file);
}




int main() {
    data_t values_A[N * M];
    int column_indices_A[N * M], row_ptr_A[N + 1], nnz_A;

    data_t values_B[M * K];
    int row_indices_B[M * K], col_ptr_B[M + 1], nnz_B;

    data_t C_ref[N][K];
    hls::vector<data_t, K> C_HLS_vec[N];

    char filename_A[50];
    snprintf(filename_A, sizeof(filename_A), "A_matrix_csr_sparsity_%.2f.bin", SPARSITY);
    read_sparse_matrix_csr(filename_A, values_A, column_indices_A, row_ptr_A, &nnz_A);

    char filename_B[50];
    snprintf(filename_B, sizeof(filename_B), "B_matrix_csc_sparsity_%.2f.bin", SPARSITY);
    read_sparse_matrix_csc(filename_B, values_B, row_indices_B, col_ptr_B, &nnz_B);

    char filename_C[50];
    snprintf(filename_C, sizeof(filename_C), "C_matrix_result_sparsity_%.2f.bin", SPARSITY);
    read_dense_matrix(filename_C, C_ref);

    // 데이터 로드 후 벡터화 (기존 코드에서 아래 코드 추가)
    vec_data_t vec_values_A[(N*M+VECTOR_SIZE-1)/VECTOR_SIZE];
    vec_idx_t vec_col_indices_A[(N*M+VECTOR_SIZE-1)/VECTOR_SIZE];

    for (int idx = 0; idx < nnz_A; idx++) {
        int batch_idx = idx / VECTOR_SIZE;
        int offset = idx % VECTOR_SIZE;
        vec_values_A[batch_idx][offset] = values_A[idx];
        vec_col_indices_A[batch_idx][offset] = column_indices_A[idx];
    }

    // B 행렬도 같은 방식으로 벡터화
    vec_data_t vec_values_B[(M*K+VECTOR_SIZE-1)/VECTOR_SIZE];
    vec_idx_t vec_row_indices_B[(M*K+VECTOR_SIZE-1)/VECTOR_SIZE];

    for (int idx = 0; idx < nnz_B; idx++) {
        int batch_idx = idx / VECTOR_SIZE;
        int offset = idx % VECTOR_SIZE;
        vec_values_B[batch_idx][offset] = values_B[idx];
        vec_row_indices_B[batch_idx][offset] = row_indices_B[idx];
    }

    // HLS kernel 호출 부분만 아래처럼 변경
    sparse_matrix_multiply_HLS(vec_values_A, vec_col_indices_A, row_ptr_A,
                            vec_values_B, vec_row_indices_B, col_ptr_B, C_HLS_vec);


    float error = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < K; j++)
            error += pow(C_HLS_vec[i][j].to_float() - C_ref[i][j].to_float(), 2);

    error /= (N * K);
    printf("MSE: %.8f\n", error);

    return 0;
}
