open_project -reset hls_prj
set_top kernel_gesummv
add_files src/gesummv.cpp
add_files src/gesummv.h
add_files -tb tb/gesummv_tb.cpp -cflags "-Isrc"
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_compile -unsafe_math_optimizations
csim_design
close_project
