open_project -reset hls_prj
set_top Gsm_LPC_Analysis
add_files add.c
add_files lpc.c
add_files private.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_rtl -reset state
source opt.tcl
csynth_design
close_project
