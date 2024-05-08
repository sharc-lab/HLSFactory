open_project -reset hls_prj
set_top md_kernel
add_files md.c
add_files md.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
source opt.tcl
csynth_design
close_project

