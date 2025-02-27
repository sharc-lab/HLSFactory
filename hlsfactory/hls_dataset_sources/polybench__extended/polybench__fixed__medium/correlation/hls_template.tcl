open_project -reset hls_prj
set_top kernel_correlation
add_files src/correlation.cpp
add_files src/correlation.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
source opt.tcl
csynth_design
close_project
