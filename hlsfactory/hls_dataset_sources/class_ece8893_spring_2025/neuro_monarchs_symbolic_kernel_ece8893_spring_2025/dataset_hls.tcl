open_project symbolic_project
set_top symbolic_circular_conv_HLS

add_files top.cpp

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 10 -name default

csynth_design
exit
