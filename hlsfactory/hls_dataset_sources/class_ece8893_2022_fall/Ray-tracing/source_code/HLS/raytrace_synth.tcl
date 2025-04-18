# TCL commands for batch-mode HLS
open_project raytrace_hls
set_top render

add_files common.cpp
add_files common.h
add_files geometry.h
add_files trianglemesh.cpp
add_files trianglemesh.h

# add_files -tb ./conv_layer_input_feature_map.bin
# add_files -tb ./conv_layer_weights.bin
# add_files -tb ./conv_layer_bias.bin
# add_files -tb ./conv_layer_output_feature_map.bin
# add_files -tb ./sim.cpp

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
