#include <stdio.h>
#include <stdlib.h>
#include <ap_fixed.h>
#include "top.h"  // Include the top function declaration

typedef ap_fixed<16, 5> data_t;

data_t DRAM_attn_input[8][256];
data_t DRAM_weights_q[256][256];
data_t DRAM_weights_k[256][256];
data_t DRAM_weights_v[256][256];
data_t DRAM_output[8][256];

void load_txt_to_array(const char *filename, data_t *array, int total_size) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Failed to open %s\n", filename);
        exit(1);
    }
    for (int i = 0; i < total_size; i++) {
        float temp;
        fscanf(fp, "%f", &temp);
        array[i] = (data_t)temp;
    }
    fclose(fp);
}

int main() {
    load_txt_to_array("DRAM_attn_input.txt", (data_t*)DRAM_attn_input, 2048);
    load_txt_to_array("DRAM_weights_q.txt", (data_t*)DRAM_weights_q, 65536);
    load_txt_to_array("DRAM_weights_k.txt", (data_t*)DRAM_weights_k, 65536);
    load_txt_to_array("DRAM_weights_v.txt", (data_t*)DRAM_weights_v, 65536);
    load_txt_to_array("DRAM_output.txt", (data_t*)DRAM_output, 2048);

    top(DRAM_attn_input, DRAM_weights_q, DRAM_weights_k, DRAM_weights_v, DRAM_output);

    // Write contents of DRAM_output to DRAM_output_output.txt
    {
        FILE *fp = fopen("DRAM_output_output.txt", "w");
        if (fp != NULL) {
            for (int i = 0; i < 2048; i++) {
                fprintf(fp, "%f ", (float)((data_t*)DRAM_output)[i]);
            }
            fclose(fp);
        }
    }

    return 0;
}