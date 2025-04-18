# Accelerating Analog CIM Simulation with FPGAs
- Final Project for Parallel Programming on FPGAs at Georgia Tech (Spring 2023)
- Authors: James Read and Vaidehi Garg

## Introduction
This repository provides the codebase for analog CIM simulations implemented using 1) Pytorch 2) Libtorch (C) and 3) Vitis HLS.

## Running Pytorch Version (Python)
1. Run the notebook `pytorch-test.ipynb` with Jupyter notebook.

## Running Libtorch Version (C)
1. Download [libtorch](https://pytorch.org/cppdocs/installing.html) and place the unzipped directory in your local copy of the github repository.
2. Navigate to the `c-libtorch` directory.
3. Update the `path_to_repo` variable in `cim_conv.cpp`. This should be the absolute path to your github repository.
4. Compile and run using the following commands:
```
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/absolute/path/to/libtorch ..
cmake --build . --config Release
./c-libtorch
```

## Running HLS Version (C Simulation and Synthesis for Baseline and Optimized Code)
1. For C simulation, navigate to `hls/baseline` or `hls/optimized`. Run `make`, then run the executable `./csim.out`.
2. For synthesis, navigate to `hls/baseline` or `hls/optimized`. Run `make synth`.

NOTE: We would prefer not to open-source our code at this time, but we hope to in the future!
