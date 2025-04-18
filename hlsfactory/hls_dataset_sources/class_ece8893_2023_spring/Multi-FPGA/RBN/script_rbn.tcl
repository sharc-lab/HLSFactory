# TCL commands for batch-mode HLS
open_project proj
set_top RBN_inf

add_files RBN_inf.cpp

#add_files -tb ../bin/conv_input.bin
#add_files -tb ../bin/conv_weights.bin
#add_files -tb ../bin/conv_bias.bin
#add_files -tb ../bin/conv_output.bin
#add_files -tb ./sim.cpp

open_solution "solution1" -flow_target vivado
set_part {xczu9eg-ffvb1156-2-e}
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
export_design -format ip_catalog

exit