#include "dcl.h"

using namespace std;

void load_tensor(const char* filename, fixed_t tensor[][N][dk], int D) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }

    size_t elements_read = fread(tensor, sizeof(fixed_t), B * N * D, file);
    if (elements_read != B * N * D) {
        fprintf(stderr, "Error reading file: %s\n", filename);
        fclose(file);
        exit(1);
    }

    fclose(file);
}


void transpose(fixed_t src[B][N][dv], fixed_t dst[B][dv][N]) {
    for (int b = 0; b < B; b++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < dv; j++) {
                dst[b][j][i] = src[b][i][j];
            }
        }
    }
}

void load_vector(fixed_t src[B][N][dv], tile_fixed_t dst[B][dv][NUM_TILES])
{
    fixed_t V_transposed[B][dv][N];
    transpose(src, V_transposed);

    for (int b = 0; b < B; b++) 
    {
        for (int n = 0; n < dv; n++)
        {
            for (int i = 0; i < NUM_TILES; i++)
            {
                for (int j = 0; j < NUM_PACKED_TILE; j++)
                {
                    dst[b][n][i][j] = V_transposed[b][n][i * NUM_PACKED_TILE + j];
                }
            }
        }
    }
}


void load_vector(fixed_t src[B][N][dk], packed_32_fixed_t dst[B][N][NUM_VECS])
{
    for (int b = 0; b < B; b++) 
    {
        for (int n = 0; n < N; n++)
        {
            for (int i = 0; i < NUM_VECS; i++)
            {
                for (int j = 0; j < NUM_PACKED_VEC; j++)
                {
                    dst[b][n][i][j] = src[b][n][i * NUM_PACKED_VEC + j];
                }
            }
        }
    }
}

int main() {
    // Allocate memory for tensors
    fixed_t Q[B][N][dk];
    fixed_t K[B][N][dk];
    fixed_t V[B][N][dv];
    fixed_t Output_ref[B][N][dv];
	fixed_t Output_HLS[B][N][dv];

    // Load tensors from binary files
    load_tensor("Q_tensor.bin", Q, dk);
    load_tensor("K_tensor.bin", K, dk);
    load_tensor("V_tensor.bin", V, dv);
	load_tensor("Output_tensor.bin", Output_ref, dv);

	for(int i = 0; i < B; i++) {
		for(int j = 0; j < N; j++) {
			for(int k = 0; k < dv; k++) {
				Output_HLS[i][j][k] = {};
			}
		}
	}

    packed_32_fixed_t Q_vecs[B][N][NUM_VECS];
    packed_32_fixed_t K_vecs[B][N][NUM_VECS];
    tile_fixed_t V_vecs[B][dv][NUM_TILES];

    load_vector(Q, Q_vecs);
    load_vector(K, K_vecs);
    load_vector(V, V_vecs);

    // call HLS kernel

    compute_attention_HLS(Q_vecs, K_vecs, V_vecs, Output_HLS);
	
	float error = 0;
	// compare HLS output and reference output tensor
	for(int i = 0; i < B; i++) {
		for(int j = 0; j < N; j++) {
			for(int k = 0; k < dv; k++) {
                error += std::pow(Output_HLS[i][j][k].to_float() - Output_ref[i][j][k].to_float(), 2);
                printf("Output_HLS[%d][%d][%d]: %.8f; Output_ref[%d][%d][%d]: %.8f\n", 
                i, j, k, Output_HLS[i][j][k].to_float(), i, j, k, Output_ref[i][j][k].to_float());
			}
		}
	}
    
	error = error / (B * N * dv);
	printf("MSE: %.8f\n", error);

    return 0;
}