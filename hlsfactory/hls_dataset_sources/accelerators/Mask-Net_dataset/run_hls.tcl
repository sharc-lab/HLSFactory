open_project Mask_Net_hls
set_top MaskNet

add_files ./src/conv1x1_new.cc
add_files ./src/dwconv3x3_new.cc
add_files ./src/dwconv5x5.cc
add_files -tb ./src/golden_c.cc
add_files ./src/net_hls.cc
add_files ./src/net_hls.h
add_files -tb ./src/output_verify.cc
add_files -tb ./src/reorder_weight.cc
add_files -tb ./src/tb.cc

add_files -tb ./test_image_bins/0.bin
add_files -tb ./test_image_bins/1.bin
add_files -tb ./test_image_bins/2.bin
add_files -tb ./test_image_bins/3.bin
add_files -tb ./test_image_bins/4.bin
add_files -tb ./test_image_bins/5.bin
add_files -tb ./test_image_bins/6.bin
add_files -tb ./test_image_bins/7.bin
add_files -tb ./test_image_bins/8.bin
add_files -tb ./test_image_bins/9.bin
add_files -tb ./test_image_bins/10.bin
add_files -tb ./test_image_bins/11.bin
add_files -tb ./test_image_bins/stitched_0_3.bin
add_files -tb ./test_image_bins/stitched_4_7.bin
add_files -tb ./test_image_bins/stitched_8_11.bin
add_files -tb ./skynet_DAC.bin


open_solution "solution1" -flow_target vivado
set_part {xczu9eg-ffvb1156-2-e}
create_clock -period 10 -name default

## C simulation
# Use Makefile instead. This is even slower.
##csim_design -O -clean

## C code synthesis to generate Verilog code
csynth_design

## C and Verilog co-simulation
## This usually takes a long time so it is commented
## You may uncomment it if necessary
##cosim_design

## export synthesized Verilog code
## export_design -format ip_catalog

exit
