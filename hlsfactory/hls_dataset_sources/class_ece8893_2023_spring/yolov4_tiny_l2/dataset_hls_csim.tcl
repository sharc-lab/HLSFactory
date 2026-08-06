open_project yolov4_tiny_l2_prj
set_top tiled_conv

add_files conv.h
add_files utils.cpp
add_files utils.h
add_files conv_7x7.cpp
add_files tiled_conv.cpp

add_files -tb sim.cpp
add_files -tb bin/conv_output_layer1.bin
add_files -tb bin/conv_output_layer2.bin
add_files -tb bin/fused_bias_layer2.bin
add_files -tb bin/fused_weights_layer2.bin
add_files -tb bin/layer1_output.bin

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
