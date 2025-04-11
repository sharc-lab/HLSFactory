open_project -reset hls_prj
set_top kernel_lu
add_files src/lu.cpp
add_files src/lu.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_compile -unsafe_math_optimizations
source opt.tcl
csynth_design
close_project
