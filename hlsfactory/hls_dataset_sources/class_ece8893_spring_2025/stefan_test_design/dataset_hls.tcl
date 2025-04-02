open_project stefan_test_design__proj

add_files my_kernel.cpp

open_solution "solution1"

set_top my_kernel

set_part {xczu3eg-sbva484-2-e}
create_clock -period 2 -name clk

csynth_design

exit