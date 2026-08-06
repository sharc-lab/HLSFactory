open_project -reset hls_prj
set_top kernel_jacobi_2d
add_files src/jacobi-2d.cpp
add_files src/jacobi-2d.h
add_files -tb tb/jacobi-2d_tb.cpp -cflags "-Isrc"
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_compile -unsafe_math_optimizations
csim_design
close_project
