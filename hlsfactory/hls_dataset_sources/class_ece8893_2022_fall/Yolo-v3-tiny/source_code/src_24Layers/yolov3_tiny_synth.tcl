# TCL commands for batch-mode HLS
open_project layer_hls
set_top yolov3_tiny

add_files conv.h
add_files utils.cpp
add_files conv_3x3_id3.cpp
add_files conv_3x3_id16.cpp
add_files conv_ver3.cpp
add_files conv_1x1.cpp
add_files yolov3_tiny.cpp

add_files -tb ./conv_layer_input_feature_map.bin
add_files -tb ./conv_layer_weights.bin
add_files -tb ./conv_layer_bias.bin
add_files -tb ./conv_layer_output_feature_map.bin
add_files -tb ./sim.cpp

open_solution "solution1" -flow_target vivado
set_part {xczu3eg-sbva484-1-e}
create_clock -period 10 -name default

## C simulation
# Use Makefile instead. This is even slower.
#csim_design -O -clean

## C code synthesis to generate Verilog code
csynth_design

## C and Verilog co-simulation
## This usually takes a long time so it is commented
## You may uncomment it if necessary
#cosim_design

## export synthesized Verilog code
#export_design -format ip_catalog

exit
