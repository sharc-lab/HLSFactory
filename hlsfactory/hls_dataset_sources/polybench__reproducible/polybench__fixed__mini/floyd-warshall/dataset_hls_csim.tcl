open_project -reset hls_prj
set_top kernel_floyd_warshall
add_files src/floyd-warshall.cpp
add_files src/floyd-warshall.h
add_files -tb tb/floyd-warshall_tb.cpp -cflags "-Isrc"
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
csim_design
close_project
