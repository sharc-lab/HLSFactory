# TCL commands for batch-mode HLS
open_project pipeline_hls
set_top img_pipeline

add_files pipeline.cpp
add_files debayer.cpp
add_files whitebalance.cpp
add_files dehaze.cpp
add_files utils.cpp

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
export_design -format ip_catalog

exit
