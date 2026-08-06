open_project neural_project
set_top last_layer_kernel

add_files top.cpp

open_solution sol1
set_part {xcvu19p-fsva3824-2-e}
create_clock -period 10 -name default

csynth_design
exit
