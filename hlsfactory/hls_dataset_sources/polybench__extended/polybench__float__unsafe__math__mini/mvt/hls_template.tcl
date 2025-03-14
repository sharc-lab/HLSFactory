open_project -reset hls_prj
set_top kernel_mvt
add_files src/mvt.cpp
add_files src/mvt.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_compile -unsafe_math_optimizations
source opt.tcl
csynth_design
close_project
