open_project -reset hls_prj
set_top kernel_gemver
add_files src/gemver.cpp
add_files src/gemver.h
add_files -tb tb/gemver_tb.cpp -cflags "-Isrc"
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_compile -unsafe_math_optimizations
csim_design
close_project
