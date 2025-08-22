#===================================
# run_hls.tcl - parameterized version
# Usage: vitis_hls -f run_hls.tcl
#===================================

# Set the module name from external substitution
set module_name @MODULE_NAME@

# Open the HLS project
open_project ${module_name}.prj -reset

# Set the top-level function of the design
set_top ${module_name}

# Add design file
add_files ${module_name}.cpp -cflags "-Isrc/ -std=c++11"

# Optionally add testbench here if needed
# add_files -tb ${module_name}_tb.cpp

# Open HLS solution
open_solution "solution1"

# Set target FPGA part (example: Alveo U280)
set_part {xcu280-fsvh2892-2L-e}

# Set clock period (e.g., 200MHz => 5ns, here it's 4ns)
create_clock -period 4

# Run C simulation
# csim_design -ldflags {-z stack-size=10485760}

# Run C synthesis
csynth_design

# Optionally run co-simulation
# cosim_design

# Close the project
close_project

# Exit Vivado HLS
quit