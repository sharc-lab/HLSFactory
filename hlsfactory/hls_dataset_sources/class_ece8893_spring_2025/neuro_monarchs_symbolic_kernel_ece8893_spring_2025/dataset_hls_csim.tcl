open_project symbolic_project
set_top symbolic_circular_conv_HLS

add_files top.cpp

add_files -tb host.cpp
add_files -tb input1_A.bin
add_files -tb input1_B.bin
add_files -tb output1_C.bin

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 10 -name default

csim_design
exit
