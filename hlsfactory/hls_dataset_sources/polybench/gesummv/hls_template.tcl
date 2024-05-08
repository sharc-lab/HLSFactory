open_project -reset hls_prj
set_top gesummv
add_files src/gesummv.c
add_files src/gesummv.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
source opt.tcl
csynth_design
close_project
