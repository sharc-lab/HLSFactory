open_project -reset hls_prj
set_top sha_stream
add_files sha.c
add_files sha.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_rtl -reset state
source opt.tcl
csynth_design
close_project

