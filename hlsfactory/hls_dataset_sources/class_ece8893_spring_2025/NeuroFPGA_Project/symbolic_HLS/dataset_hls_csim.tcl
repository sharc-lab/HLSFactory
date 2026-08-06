open_project project_1
set_top circular_convolution_3d

add_files top.cpp

add_files -tb host.cpp
add_files -tb input1_A.bin
add_files -tb input1_B.bin
add_files -tb output1_C.bin

open_solution sol1
set_part {xave1752-vsva1596-1LJ-i-S}
create_clock -period 10 -name default

csim_design
exit
