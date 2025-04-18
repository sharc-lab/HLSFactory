# Ray Tracing Acceleration on FPGA
## Goal
The goal of the project is to accelerate the ray cast and primitive intersection operations in the ray tracing pipeling on the Pynq Z2 board using high level synthesis (HLS) techniques.

### Course
Course: ECE 8893-FPG: Parallel Programming for FPGAs

### Contributors
Varun Saxena (varunsax12 | https://github.com/varunsax12) </br>
Santhana Bharathi Narasimmachari (nsbneyveli | https://github.com/nsbneyveli) </br>
Venkata Hanuma Sandilya Balemarthy (sandilyabvh | https://github.com/sandilyabvh) </br>

## Code Description
The code reads a .geo file [1] which defines the structure of the image in terms of faces, primitives, vertices, etc. This data is parsed and stored. The render function then casts ray to each pixel in the framebuffer from the camera view (origin). The rays are then tested for intersections with all the primitives. The primitive with the closest point of intersection is considered the main intersection. This point is then used to find the texture coordinates fetched from the .geo file. The texture is then applied to the pixel and stored back in the frame buffer. This process is done iteratively for each pixel in the frame buffer.

## HLS optimization techniques used:
1. C level code speed up
2. Fixed point number (with precision decided based on test runs)
3. Storing data in BRAM (primitive buffer and tranformation matrices)
4. Array partition of the BRAM data
5. Loop unrolling
6. Pipelining
7. Inlining functions
8. Reordering BRAM structure to enable burst operation from the DRAM
9. Modified modulus and sqrt [2] operation to reduce DSP usage
10. Dataflow optimization for casting 3 rays in parallel
    a. Additional BRAM supporting structures for this.

## Directory Structure

|-golden_c : C code for the ray tracing implementation </br>
|-HLS      : HLS optimized code for ray tracing targetted for Pynq Z2 board </br>

## Code Structure

### Ray Tracing Core:
trianglemesh.cpp : Complete ray tracing algorithm (Header: trainglemesh.h) </br>
geometry.h : Configurations of workload and other top level run configs </br>
common.cpp : Custom utilities used in the ray tracing algorithm (Header: common.h) </br>

### Testbench files
main.cpp : Top testbench file with functionality to read and process the ".geo" file to generate index buffer, primitive buffer, texture coordinates, normals, etc. It also applies the preset transformations. </br>
tb_common.cpp : Custom utilities used in the processing the ".geo" file. (HEader: tb_common.h) </br>

### Input Files
Geometry file: teapot.geo

## Test Environment
NOTE: Needs g++ and vitis installed
Server: ece-linlabsrv01.ece.gatech.edu

Environment Setup:
1. source /tools/software/xilinx/setup_env.sh
2. source /tools/software/xilinx/Vitis_HLS/2021.1/settings64.sh
3. alias vitis_hls="/tools/software/xilinx/Vitis_HLS/2021.1/bin/vitis_hls"
4. alias vivado="/tools/software/xilinx/Vivado/2021.1/bin/vivado"

## Run Instructions

### Golden C
1. cd golden_c
2. make clean
3. make
4. make run

Expected Output: out.0000.ppm file (displaying the teapot image with checkered texture)

### HLS (floating point simulations)
1. cd HLS
2. ./csim.sh
3. make run

Expected Output: out.0000.ppm file (displaying the teapot image with checkered texture)

### HLS (fixed point simulations)
1. cd HLS
2. ./csim_f.sh
3. make run

Expected Output: out.0001.ppm file (displaying the teapot image with checkered texture)

### HLS synthesis run (fixed point)
1. cd HLS
2. ./csynth.sh

Expected Output: raytrace_hls folder containing the HLS run results


# References
[1] https://www.scratchapixel.com/lessons/3d-basic-rendering/transforming-objects-using-matrices </br>
[2] https://github.com/Xilinx/Vitis-HLS-Introductory-Examples/tree/master/Modeling/fixed_point_sqrt
