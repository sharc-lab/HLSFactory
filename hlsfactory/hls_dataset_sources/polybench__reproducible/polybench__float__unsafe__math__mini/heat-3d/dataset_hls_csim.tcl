open_project -reset hls_prj
set_top kernel_heat_3d
add_files src/heat-3d.cpp
add_files src/heat-3d.h
add_files -tb tb/heat-3d_tb.cpp -cflags "-Isrc"
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_compile -unsafe_math_optimizations
csim_design
close_project
