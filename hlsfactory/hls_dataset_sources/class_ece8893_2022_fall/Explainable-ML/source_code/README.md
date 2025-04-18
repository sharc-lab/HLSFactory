python/
Contains the python scripts to train the model, write the network parameters into binary files and to run the forward and backward pass to generate the ground truth for the cmodel implementation. Also contains the .pth file for the model itself as well as ground truth outputs in .npy format. 
Requirements: torch, numpy

bin/
Contains the binary files for the network parameters for different layers

cmodel/
Contains header files for template functions that run the forward pass and backward pass through the network

tiled_conv.cpp : top file containing tiled implementation of CNN forward pass followed by backward pass
top_synth.tcl  : TCL file to synthesize tiled_conv.cpp

sim.cpp : testbench that can run both the cmodel and tiled_conv (tiled version of cmodel which can fit on the FPGA)

gradient.h  : header file containing data types and various parameters for choosing the tile dimensions
io.h        : header file containing templated functions to load the network parameter values in the testbench
utils.h     : header file containing templated functions to implement the tiled verison of cmodel on the FPGA

Project Contributors:
1) Ashwin Bhat
2) Adou Sangbone Assoa