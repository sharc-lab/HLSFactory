open_project yolov4_tiny_csp_l12_prj
set_top yolov4_tiny

add_files conv.h
add_files utils.cpp
add_files yolov4_tiny.cpp

add_files -tb sim.cpp
add_files -tb conv_layer1_bias.bin
add_files -tb conv_layer1_input.bin
add_files -tb conv_layer1_output.bin
add_files -tb conv_layer1_weights.bin
add_files -tb conv_layer2_bias.bin
add_files -tb conv_layer2_output.bin
add_files -tb conv_layer2_weights.bin

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
