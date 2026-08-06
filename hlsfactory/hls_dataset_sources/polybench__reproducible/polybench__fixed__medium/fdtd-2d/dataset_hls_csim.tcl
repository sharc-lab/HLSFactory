open_project -reset hls_prj
set_top kernel_fdtd_2d
add_files src/fdtd-2d.cpp
add_files src/fdtd-2d.h
add_files -tb src/fdtd-2d_tb.cpp -cflags "-Isrc"
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
csim_design
close_project
