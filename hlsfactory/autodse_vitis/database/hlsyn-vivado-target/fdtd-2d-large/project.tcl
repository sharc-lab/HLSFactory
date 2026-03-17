# Set project name and path
set project_name "project"
set src_path "." 

# Create a new HLS project
open_project -reset "$src_path/$project_name"

# Add C++ source files
add_files "testbench.cpp"
add_files "fdtd-2d-large.cpp"
add_files "fdtd-2d-large.h"
add_files -tb "testbench.cpp"

open_solution "solution"
set_part $env(TARGET_PART_NUM)
set_top "kernel_fdtd_2d"
# Set the target frequency
create_clock -period [expr {1000.0 / $env(TARGET_FREQ)}]
config_compile -unsafe_math_optimizations 

# Run the simulation
csynth_design
# export_design -flow syn -rtl verilog  
close_project

exit
