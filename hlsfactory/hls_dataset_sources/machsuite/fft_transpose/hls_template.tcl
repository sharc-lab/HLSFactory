open_project -reset hls_prj
set_top fft1D_512
add_files fft.c
add_files fft.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
source opt.tcl
csynth_design
close_project

