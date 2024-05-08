open_project -reset hls_prj
set_top k2mm
add_files src/k2mm.c
add_files src/k2mm.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
source opt.tcl
csynth_design
close_project
