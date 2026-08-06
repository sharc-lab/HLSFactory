open_project neural_project
set_top last_layer_kernel

add_files top.cpp

add_files -tb host.cpp
add_files -tb input_1.bin
add_files -tb last_layer_weights_clean.bin
add_files -tb output_1.bin

open_solution sol1
set_part {xcvu19p-fsva3824-2-e}
create_clock -period 10 -name default

csim_design
exit
