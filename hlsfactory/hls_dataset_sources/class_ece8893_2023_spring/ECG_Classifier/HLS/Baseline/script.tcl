# TCL commands for batch-mode HLS
open_project proj
set_top tiled_conv

add_files conv.h
add_files conv1d.cpp

#add_files -tb bin/conv1.bin
#add_files -tb bin/conv1_weights.bin
#add_files -tb bin/conv1_bias.bin
#add_files -tb bin/conv2.bin
#add_files -tb bin/conv2_weights.bin
#add_files -tb bin/conv2_bias.bin
#add_files -tb bin/conv3.bin
#add_files -tb bin/conv3_weights.bin
#add_files -tb bin/conv3_bias.bin
#add_files -tb bin/dense1_bias.bin
#add_files -tb bin/dense1_weights.bin
#add_files -tb bin/dense2_bias.bin
#add_files -tb bin/dense2_weights.bin

#add_files -tb sim.cpp

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
