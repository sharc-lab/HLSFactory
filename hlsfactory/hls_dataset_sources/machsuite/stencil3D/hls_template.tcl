open_project -reset hls_prj
set_top stencil3d
add_files stencil.c
add_files stencil.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
source opt.tcl
csynth_design
close_project

