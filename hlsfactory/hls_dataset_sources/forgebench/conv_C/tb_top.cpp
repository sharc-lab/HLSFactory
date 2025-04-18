#include <stdio.h>
#include <stdlib.h>
#include <ap_fixed.h>
#include "top.h"  // Include the top function declaration

typedef ap_fixed<16, 5> data_t;

data_t DRAM_image_input[128][14][14];
data_t DRAM_conv_weight[128][128][3][3];
data_t DRAM_conv_bias[128];
data_t DRAM_image_output[128][14][14];

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
    load_txt_to_array("DRAM_image_input.txt", (data_t*)DRAM_image_input, 25088);
    load_txt_to_array("DRAM_conv_weight.txt", (data_t*)DRAM_conv_weight, 147456);
    load_txt_to_array("DRAM_conv_bias.txt", (data_t*)DRAM_conv_bias, 128);
    load_txt_to_array("DRAM_image_output.txt", (data_t*)DRAM_image_output, 25088);

    top(DRAM_image_input, DRAM_conv_weight, DRAM_conv_bias, DRAM_image_output);

    // Write contents of DRAM_image_output to DRAM_image_output_output.txt
    {
        FILE *fp = fopen("DRAM_image_output_output.txt", "w");
        if (fp != NULL) {
            for (int i = 0; i < 25088; i++) {
                fprintf(fp, "%f ", (float)((data_t*)DRAM_image_output)[i]);
            }
            fclose(fp);
        }
    }

    return 0;
}