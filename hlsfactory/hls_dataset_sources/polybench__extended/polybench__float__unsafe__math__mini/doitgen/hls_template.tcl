open_project -reset hls_prj
set_top kernel_doitgen
add_files src/doitgen.cpp
add_files src/doitgen.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_compile -unsafe_math_optimizations
source opt.tcl
csynth_design
close_project
