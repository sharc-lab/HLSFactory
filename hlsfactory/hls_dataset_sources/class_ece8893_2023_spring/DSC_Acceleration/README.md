# Accelerating Depthwise Separable Convolution using HLS
The code is an HLS Implementation of Depthwise Separable Convolution. We also have included the Python Code to generate the required binary files for verifying our HLS implementation.

## How to use
To generate weights, inputs and output bin files as were used, run the *MobileNet_weights.ipynb* Jupyter notebook.

Requirements for jupyter notebook:
+ Torch
+ Torchvision

1. Run `make` in the code folder `optimized` or `unoptimized` to generate the executable files. 
1. Then, run `./csim.out` to verify implementation. 

Run `make synth` in the `optimized` or `unoptimized` folder to generate Vitis HLS estimates. 

If you prefer not to generate Vivado IP, comment out
`export_design -format ip_catalog` in the `script.tcl` file.

DSC folder contains all the weights, inputs and outputs bin files required to run the HLS code.

**Open to reusing the code.** 