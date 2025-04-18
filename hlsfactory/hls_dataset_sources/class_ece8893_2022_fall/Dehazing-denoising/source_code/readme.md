# README (Image Processing Pipeline)

This code implements the Image Processing Pipeline in fullfillment of the final project for ECE 8893 - Spring 2022 - Parallel Programming for FPGAs.

The code runs C simulation for the image pipeline and provides a script to run HDL synthesis in Vitis HLS.

## Requirements

Can be run in a standard Linux environment with Vitis HLS installed. No code references are used for this project (papers are listed in the report).

The instructors may use the code as they please. We are willing to open-source our code.

## HOW TO RUN: ##

# 1. Make
```
cd ./src
make clean
make
```
# 2. Running C-simulation
C-simulation results are stored in image_out.raw and converted to result.png
```
./result
```

# 3. Running Vitis HLS
```
lastyear vitis_hls -f top_csynth.tcl
```

# Miscellaneous:
To convert a raw file to png:  
```
convert -size 512x384 -depth 8 RGB:image_out.raw result.png
```
