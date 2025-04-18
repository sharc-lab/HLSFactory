#include <stdio.h>
#include <stdlib.h>
#include <ap_fixed.h>
#include "top.h"  // Include the top function declaration

typedef ap_fixed<16, 5> data_t;

data_t DRAM_1[16][8];
data_t DRAM_2[8][32];
data_t DRAM_3[16][32];
data_t DRAM_4[16][32];
data_t DRAM_5[32];
data_t DRAM_10[32];
data_t DRAM_11[1];
data_t DRAM_12[1];

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
    load_txt_to_array("DRAM_1.txt", (data_t*)DRAM_1, 128);
    load_txt_to_array("DRAM_2.txt", (data_t*)DRAM_2, 256);
    load_txt_to_array("DRAM_3.txt", (data_t*)DRAM_3, 512);
    load_txt_to_array("DRAM_4.txt", (data_t*)DRAM_4, 512);
    load_txt_to_array("DRAM_5.txt", (data_t*)DRAM_5, 32);
    load_txt_to_array("DRAM_10.txt", (data_t*)DRAM_10, 32);
    load_txt_to_array("DRAM_11.txt", (data_t*)DRAM_11, 1);
    load_txt_to_array("DRAM_12.txt", (data_t*)DRAM_12, 1);

    top(DRAM_1, DRAM_2, DRAM_3, DRAM_4, DRAM_5, DRAM_10, DRAM_11, DRAM_12);

    // Write contents of DRAM_4 to DRAM_4_output.txt
    {
        FILE *fp = fopen("DRAM_4_output.txt", "w");
        if (fp != NULL) {
            for (int i = 0; i < 512; i++) {
                fprintf(fp, "%f ", (float)((data_t*)DRAM_4)[i]);
            }
            fclose(fp);
        }
    }

    // Write contents of DRAM_12 to DRAM_12_output.txt
    {
        FILE *fp = fopen("DRAM_12_output.txt", "w");
        if (fp != NULL) {
            for (int i = 0; i < 1; i++) {
                fprintf(fp, "%f ", (float)((data_t*)DRAM_12)[i]);
            }
            fclose(fp);
        }
    }

    return 0;
}