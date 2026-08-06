open_project -reset hls_prj
set_top kernel_durbin
add_files src/durbin.cpp
add_files src/durbin.h
add_files -tb tb/durbin_tb.cpp -cflags "-Isrc"
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
csim_design
close_project
