# Purpose

This is a code for implementation of the Lucas Kanade algorithm for Optical Flow measurement. The input consists of two 100x100 sized images, and the outputs are three text files, giving motion vectors in the X, Y direction, and motion compensated image pixel values.

# Environment Setup

The Vitis inside HLS folder consists of files needed for an HLS implementation and the optimized algorithm and generate an RTL on VitisHLS and synthesis. Under source files, ref_LK.cpp should be added. This has the top function "ref_LK.cpp".
Under test bench, "LKof_main.cpp" and the other header files are to be added. 
ref_LK.main.cpp is the file that has been optimized for an FPGA implementation, using pragmas
design_1.bit, design_1.hwh are vivado generated files, for running the optimized code on  the pynqZ2 FPGA using a python testbench (JupyterNotebook.ipynb)
ref_LK_csynth.rpt is the optimized Vitis synthesis report for our best latrncy anf utilization values.
The Golden C folder consists of the reference C code (reference: Xilinx)

The Python folder contains the test bench for running the vivado generatad block on a virtual FPGA pynqZ2 (JupyterNotebook.ipynb). The lk_inp.py file contains a python implementation of Lucas Kanade. It also contains the input images, and results obtained using a MATLAB testbench.
GoldenC is the C code used as reference 

# Reference code
Application note: Demystifying the Lucas-Kanade Optical
Flow Algorithm with Vivado HLS
https://docs.xilinx.com/v/u/en-US/xapp1300-lucas-kanade-optical-flow


 