# TCL commands for batch-mode HLS
open_project proj
set_top yolov4_tiny

add_files conv.h
add_files utils.cpp
add_files yolov4_tiny.cpp

#add_files -tb ../bin/conv_input.bin
#add_files -tb ../bin/conv_weights.bin
#add_files -tb ../bin/conv_bias.bin
#add_files -tb ../bin/conv_output.bin
#add_files -tb ./sim.cpp

open_solution "solution1" -flow_target vivado
set_part {xc7z020clg400-1}
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
