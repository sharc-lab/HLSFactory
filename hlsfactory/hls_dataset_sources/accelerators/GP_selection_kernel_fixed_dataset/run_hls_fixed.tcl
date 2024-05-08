# TCL commands for batch-mode HLS

# Create project
open_project GP_selection_kernel_fixed

# Set top-level design file (DUT)
set_top GP_kernel_fixed

# Add source code files
add_files ./src/GP_kernel_100_10_success_fixed.cpp

# Add test bench files
add_files -tb ./src/testbench_fixed.cpp
add_files -tb ./src/a_float.txt
add_files -tb ./src/beta_float.txt
add_files -tb ./src/mu_float.txt
add_files -tb ./src/sigma_float.txt

# Create design solution
open_solution "solution1" -flow_target vivado

# Set the FPGA board
set_part {xczu9eg-ffvb1156-2-e}

# Set the clock period
create_clock -period 10 -name default

## C simulation
# Use Makefile instead. This is even slower.
# csim_design -O -clean

## C code synthesis to generate Verilog code
csynth_design

## C and Verilog co-simulation
## This usually takes a long time so it is commented
## You may uncomment it if necessary
## cosim_design -trace_level all

## export synthesized Verilog code
## export_design -format ip_catalog

exit