# TCL commands for batch-mode HLS
open_project hls_saliency_map
set_top tiled_conv

add_files tiled_conv.cpp

open_solution "solution1" -flow_target vivado

#Ultra96
#set_part {xczu3eg-sbva484-1-e}  

# Pynq-Z2
set_part {xc7z020clg400-1}      

# Virtex Family Board
#set_part {xc7v2000tfhg1761-1}  

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
