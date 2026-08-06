open_project -reset hls_prj
set_top kernel_mvt
add_files src/mvt.cpp
add_files src/mvt.h
add_files -tb tb/mvt_tb.cpp -cflags "-Isrc"
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
csim_design
close_project
