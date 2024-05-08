############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project SkyNet_hls
set_top SkyNet

add_files ./src/conv1x1.cc
add_files ./src/dwconv3x3.cc
add_files ./src/net_hls.cc
add_files ./src/net_hls.h
add_files -tb ./test_image_bins/0.bin
add_files -tb ./test_image_bins/1.bin
add_files -tb ./test_image_bins/10.bin
add_files -tb ./test_image_bins/11.bin
add_files -tb ./test_image_bins/2.bin
add_files -tb ./test_image_bins/3.bin
add_files -tb ./test_image_bins/4.bin
add_files -tb ./test_image_bins/5.bin
add_files -tb ./test_image_bins/6.bin
add_files -tb ./test_image_bins/7.bin
add_files -tb ./test_image_bins/8.bin
add_files -tb ./test_image_bins/9.bin
add_files -tb ./src/golden_c.cc
add_files -tb ./src/output_verify.cc
add_files -tb ./src/reorder_weight.cc
add_files -tb ./test_image_bins/stitched_0_3.bin
add_files -tb ./test_image_bins/stitched_4_7.bin
add_files -tb ./test_image_bins/stitched_8_11.bin
add_files -tb ./src/tb.cc
add_files -tb weights_floating.bin
open_solution "solution1" -flow_target vivado
set_part {xczu9eg-ffvb1156-2-e}
create_clock -period 10 -name default

## C simulation
##csim_design

## C code synthesis to generate Verilog code
csynth_design

## C and Verilog co-simulation
## This usually takes a long time so it is commented
## You may uncomment it if necessary
#cosim_design

## export synthesized Verilog code
## export_design -format ip_catalog

exit
