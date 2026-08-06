open_project -reset hls_prj
set_top kernel_gramschmidt
add_files src/gramschmidt.cpp
add_files src/gramschmidt.h
add_files -tb tb/gramschmidt_tb.cpp -cflags "-Isrc"
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_compile -unsafe_math_optimizations
csim_design
close_project
