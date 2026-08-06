open_project -reset hls_prj
set_top kernel_bicg
add_files src/bicg.cpp
add_files src/bicg.h
add_files -tb tb/bicg_tb.cpp -cflags "-Isrc"
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_compile -unsafe_math_optimizations
csim_design
close_project
