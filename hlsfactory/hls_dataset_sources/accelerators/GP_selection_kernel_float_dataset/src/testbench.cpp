#include <stdio.h>
#include <stdlib.h>

#include <vector> 
#include <algorithm>
#include <string.h>
#include <iostream>
#include <cstddef>

#include <math.h>
#include <ap_fixed.h>
#include "hls_stream.h"
#include "hls_math.h"

#include <fstream>
#include <cmath>

#include <ap_int.h>



// #define N 128 //The number of clients
// #define C 10 //The number of clients to be selected
// #define SORT_NUM 32

// #define N_CLIENT 72

// #define BUFFER_SIZE 64
// #define UNROLL_FACTOR 16

#define N 100 //The number of clients
#define C 10 //The number of clients to be selected
#define SORT_NUM 32

#define N_CLIENT 100

#define BUFFER_SIZE 20
#define UNROLL_FACTOR 20

#define SCALE 5



typedef float DATA_TYPE;
typedef float Key_Type;
//typedef ap_fixed<32,10> DATA_TYPE;
//typedef ap_fixed<32,10> Key_Type;
typedef struct {
    Key_Type number;
    int index;
} Data_Sort;

// define some on-chip buffers


void readFloatsFromFile_1d(const char *filename, float numbers[], int size) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size && fscanf(file, "%f", &numbers[i]) == 1; i++) {
        // Reading numbers
    }

    fclose(file);
}


void readFloatsFromFile_2d(const char *filename, float numbers[N][N], int rows, int cols) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fscanf(file, "%f", &numbers[i][j]) != 1) {
                fprintf(stderr, "Error reading number at [%d][%d]\n", i, j);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);
}




extern void GP_kernel_float(DATA_TYPE *mu_dram,  DATA_TYPE sigma_dram[N][N], DATA_TYPE *a_dram, DATA_TYPE *beta_dram, int client_number_array[10], int client_index_list[C], DATA_TYPE result_dram[N][N], DATA_TYPE result_dram_2[N]);

int main()

{

    // Seed the random number generator with the current time
    //srand(333);

    // Define the size of the array
    int arraySize = N_CLIENT; // Change this to the desired size
    int client_num = 10; //Change between 1-10 in the float point version

    // Create an array to store the random numbers
    DATA_TYPE mu_dram[N];
    DATA_TYPE a_dram[N];
    DATA_TYPE sigma_dram[N][N];
    DATA_TYPE beta_dram[N];

    int client_index_list[C];

    DATA_TYPE result_dram[N][N];
    DATA_TYPE result_dram_2[N];


    float mu_float[N_CLIENT];
    float a_float[N_CLIENT];
    float sigma_float[N_CLIENT][N_CLIENT];
    float beta_float[N_CLIENT];
    float result_float[N_CLIENT][N_CLIENT];
    float result_float_2[N_CLIENT];
    int client_number_array[10];
    client_number_array[0] = client_num;



    // Generate random numbers in the range [0, 1)

    readFloatsFromFile_1d("mu_float.txt", mu_float, N_CLIENT);
    readFloatsFromFile_1d("a_float.txt", a_float, N_CLIENT);
    readFloatsFromFile_1d("beta_float.txt", beta_float, N_CLIENT);
    readFloatsFromFile_2d("sigma_float.txt", sigma_float, N_CLIENT, N_CLIENT);

    // for (int i = 0; i < arraySize; i++) {
    //     mu_float[i] = (float)rand() / RAND_MAX;
    //     printf("the value of mu_float is: %f\n", mu_float[i]);
        
    // }

    // for (int i = 0; i < arraySize; i++) {
    //     a_float[i] = (float)rand() / RAND_MAX;
        
    // }

    // for (int i = 0; i < arraySize; i++) {
    //     beta_float[i] = (float)rand() / RAND_MAX;
        
    // }

    // for (int i = 0; i < arraySize; i++) {
    //     for (int j = 0; j < arraySize; j++)
    //     {
    //         sigma_float[i][j] = (float)rand() / RAND_MAX;
    //     }
        
    // }

    for (int i = 0; i < N; i++)
    {
        mu_dram[i] = 0;
        a_dram[i] = 0;
        beta_dram[i] = 0;
        for (int j = 0; j < N; j++)
        {
            sigma_dram[i][j] = 0;
        }
    }


    for (int i = 0; i < arraySize; i++)
    {
        mu_dram[i] = (DATA_TYPE) mu_float[i];
        a_dram[i] = (DATA_TYPE) a_float[i];
        beta_dram[i] = (DATA_TYPE) beta_float[i];
        for (int j = 0; j < arraySize; j++)
        {
            sigma_dram[i][j] = (DATA_TYPE) sigma_float[i][j];
        }
    }




    GP_kernel(mu_dram, sigma_dram, a_dram, beta_dram, client_number_array, client_index_list, result_dram, result_dram_2);

    // Print the generated array
    // printf("Random numbers in the range [0, 1):\n");
    // printf("mu_dram:\n");
    // for (int i = 0; i < arraySize; i++) {
    //     printf("%.6f\n", mu[i]);
    // }
    // printf("a_dram:\n");
    // for (int i = 0; i < arraySize; i++) {
    //     printf("%.6f\n", a[i]);
    // }
    // printf("beta_dram:\n");
    // for (int i = 0; i < arraySize; i++) {
    //     printf("%.6f\n", beta[i]);
    // }
    // printf("sigma_dram:\n");
    // for (int i = 0; i < arraySize; i++) {
    //     for (int j = 0; j < arraySize - 1; j++)
    //     {
    //         printf("%.6f\n", sigma_12[i][j]);
    //     }
        
    // }

    // printf("delta_l:\n");
    // for (int i = 0; i < arraySize; i++) {
    //     printf("%d\n", non_selected_client_index_table[i]);
    // }

    for (int i = 0; i < arraySize; i++)
    {
        result_float_2[i] = (float) result_dram_2[i];
        for (int j = 0; j < arraySize; j++)
        {
            result_float[i][j] = (float) result_dram[i][j];
        }
        
    }

   printf("client_index_list:\n");
     for (int i = 0; i < C; i++) {
         printf("%d\n", client_index_list[i]);
     }


    printf("mu updated:\n");
    for (int i = 0; i < arraySize; i++) {
        printf("%.6f\n", result_float_2[i]);
    }
    // printf("sigma_22:\n");
    // for (int i = 0; i < arraySize; i++) {
    //     for (int j = 0; j < arraySize; j++)
    //     {
    //         printf("%.6f\n", sigma_dram[i][j]);
    //     }
        
    // }
    
    return 0;
}
