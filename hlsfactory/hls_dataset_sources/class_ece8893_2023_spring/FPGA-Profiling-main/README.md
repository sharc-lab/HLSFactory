## Disclaimer
The code is open source and can be used for modifications and improvement, without any guarantee or waranty from the authors.

# FPGA profiling need and motivation
## Summary

The project included multiple type of codes to perfrom certain desgnated operations/functions with different optimastions. The aim of the project was to gather a sample space of codes to establish that there is a vast difference in latency numbers reported by HLS and the true numbers. The code included in this repository is only of the function with maximum difference found.

## Apply Watermark function

This code takes in a 920x549 image and applys a grid of 16x16 gray "X" as a watermark. The reference code was taken from the git repository of [Vitits Accel Examples](https://github.com/Xilinx/Vitis_Accel_Examples) 
There were different optimisations applied to the basic code to gather a diverse sample space with a reation as to what could be the speed up differences. Another variation of data type (16 bit and 32 bit) was also added.

## How to execute the code

### HLS code

1. Clone the [Vitits Accel Examples](https://github.com/Xilinx/Vitis_Accel_Examples) from the link.
2. Copy the cpp kernels and test bench in the path "\Vitis_Accel_Examples\cpp_kernels\critical_path\src".
3. Source the environment setup file from your server.
4. Use the respective kernel code and test bench(16 bit kernels with 16 bit test bech and 32 bit with 32 bit) from the location to make a Vitis HLS project.
5. Generate the report for Csynth and Co-sim.

### Host code

The host code .ipynb files are placed in the path "Python\"
1. Copy the folder to synestia server.
2. Run the respective .ipnyb file to get the results of the HW runs.

## References
1. [Vitits Accel Examples](https://github.com/Xilinx/Vitis_Accel_Examples)
