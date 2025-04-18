# Acceleration of YOLOv4-Tiny backbone using HLS
>***Team Members: Anusha Sumbetla, Sibi Renganatth Sudhakar, Vishwanath Kondepudi***


Our project aims to speed up the backbone architecture of YOLOv4-Tiny object algorithm using HLS. We have used Google colab to build our Yolov4-Tiny Pytorch model and generated individual layer weights from the model. Since batch normalization is tough to run with the FPGA resources, we have opted to combine the convolution and batch normalization layers into one single layer.

Our base is the Yolov4-Tiny algorithm which is discussed in the paper [https://arxiv.org/abs/2011.04244]. We have tried to implement the first 6 layers from the paper, i.e, the first two Conv2D layers and a CSP Block.

Fusing of the layers is currently done for two Conv2D layers and the first two layers of CSP block. We are looking to implement them on board to see the performance overheads.

For starter, the Pytorch code is self explanatory and can be easier to proceed with. The code could work as a base for any object detection or classification algorithm.

We have not implemented Golden C as the Pytorch model was sufficient and we had the convolution layers available already.


## Directory Structure:
 + **bin/**
 Directory containing binary files such as convolution layer weights, bias, batch normalization parameters. We have the input image converted to binary. More test images can be used based on the Python code.
 + **Python/**
 Directory containing Jupyter Notebook for running
 + **HLS_C/**
 HLS C++ implementation of the individual layers and the fused layers - Individual layers as well as the Fused Layers are present as separate projects, for ease of running
 + **Synth_Report/**
 Directory containing reports of all the individual layers
 
 ## Execution step - Individual/Fused Layers:
 
 1. source run_script.sh

 2. cd HLS_C

 3. make clean; make all; make csimout; - To view MSE simulations with floating point

 4. make clean; make hls_sim; make csimout - To view FixedPoint simulations.

 5. Individual layer MSE can be viewed and individual layer outputs can be viewed.

***Note: The binary files are currently required to be within the directory of the individual layers. To change it, please update the sim.cpp file with the correct directory and file name***

