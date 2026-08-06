open_project dual_interface_FIFO_receive_prj
set_top dual_interface_FIFO_receive

add_files dual_interface_FIFO_receive.cpp -cflags "-Isrc/ -std=c++11"

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 4 -name default

csynth_design
exit
