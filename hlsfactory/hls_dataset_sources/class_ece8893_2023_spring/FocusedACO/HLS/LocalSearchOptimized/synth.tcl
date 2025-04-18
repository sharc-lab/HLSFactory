# TCL commands for batch-mode HLS

# Create project
open_project proj

# Set top-level design file (DUT)
set_top two_opt_nn

# Add source code files
add_files two_opt_nn.cpp

# Add test bench files
add_files -tb main.cpp

# Create design solution
open_solution "solution2"

# Set the FPGA board
set_part {xc7z020clg400-1}

# Set the clock period
create_clock -period 10 -name default

## C simulation
# Use Makefile instead. This is even slower.
#csim_design -O -clean

## C code synthesis to generate Verilog code
csynth_design

## C and Verilog co-simulation
## This usually takes a long time so it is commented
## You may uncomment it if necessary
cosim_design

## export synthesized Verilog code
# export_design -format ip_catalog

exit
