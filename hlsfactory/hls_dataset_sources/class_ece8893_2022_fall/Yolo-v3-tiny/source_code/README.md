# ECE8893: FPGA Acceleration of Yolov3-Tiny
> ***Team Members:*** Anshuman, Arvind Nataraj Sivasankar, Shreyas Tater

This source code is for acceleration of yolov3-tiny for object detection using an FPGA (xczu3eg-sbva484-1-e) with the help of different acceleration techniques such as array partitioning, loop pipelining, loop unroll, loop tiling and ping pong buffers.

## DIRECTORY STRUCTURE

 + **bin/**
 Directory containing binary files such as convolution layer weights, bias, batch normalization parameters etc
	 + **bias/**
	 Bias of convolution layers 10 and 13 
	 + **bn/**
	 Batch Normalization parameters of convolution layers 1-9, 11-12
	 + **conv/**
	 Weights of convolution layers 1-13
	 + **img/**
	 Input image in binary format
	 + **yolov3-tiny.weights**
	 Weights file from which bias, batch normalization parameters and convolution weights are extracted
 + **images/**
 Images to be detected (in JPG format)
 + **reports/**
 4-layer and all-layer vitis final synthesis report
 + **scripts/**
	 + **read_image_input.mlx**
	 Matlab script to convert image from .jpg to binary format
	 + **read_weights.mlx**
	 Matlab script to read yolov3-tiny.weights file and create individual binary weights files for each layer and type of weight (convolution, bias, batch normalization)
	 + **setup_env.sh**
	 Script to setup vitis/vivado environment
 + **src_4Layers/**
 Working directory of 4-layer FPGA acceleration (includes both C-Model and HLS)
 + **src_24Layers/**
 Working directory of all-layer (24 layers) FPGA acceleration (includes both C-Model and HLS)

## EXECUTION STEPS - 4 LAYERS

### Steps to run 4-layer C-model(float)

1. cd src_4Layers/

2. make clean

3. source 0_cmodel_sim_float.sh

4. ./csim.out

>*NOTE:* To run fixed point C-model, run "1_cmodel_sim_fixp.sh" in step 3 above

### Steps to run synthesis of 4-layers of yolov3-tiny structure(fixed point)

1. source scripts/setup_env.sh (vitis/vivado setup)

2. cd src_4Layers/

3. make clean

4. source 3_hls_sim_fixp.sh (build)

5. ./csim.out (to ensure functional correctness)

6. lastyear vitis_hls -f yolov3_tiny_synth.tcl (to start synthesis)

## EXECUTION STEPS - ALL LAYERS

### Steps to run all-layer C-model(float)

1. cd src_24Layers/

2. make clean

3. source 0_cmodel_sim_float.sh

4. ./csim.out

>*NOTE:* To run fixed point C-model, run "1_cmodel_sim_fixp.sh" in step 3 above

### Steps to run synthesis of all layers of yolov3-tiny structure(fixed point)

1. source scripts/setup_env.sh (vitis/vivado setup)

2. cd src_24Layers/

3. make clean

4. source 3_hls_sim_fixp.sh (build)

5. ./csim.out (to ensure functional correctness)

6. lastyear vitis_hls -f yolov3_tiny_synth.tcl (to start synthesis)

