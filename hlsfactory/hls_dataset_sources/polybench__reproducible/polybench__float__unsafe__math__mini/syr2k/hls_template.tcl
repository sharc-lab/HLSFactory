open_project -reset hls_prj
set_top kernel_syr2k
add_files src/syr2k.cpp
add_files src/syr2k.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_compile -unsafe_math_optimizations
source opt.tcl
csynth_design
close_project
