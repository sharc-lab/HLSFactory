open_project yolov4_tiny_l5_prj
set_top tiled_conv

add_files conv.h
add_files utils.cpp
add_files utils.h
add_files conv_7x7.cpp
add_files tiled_conv.cpp

add_files -tb sim.cpp
add_files -tb bin/conv_layer5_input.bin
add_files -tb bin/conv_layer5_output.bin
add_files -tb bin/fused_conv5_bn5_bias2.bin
add_files -tb bin/fused_conv5_bn5_weights2.bin
add_files -tb bin/layer3_output.bin
add_files -tb bin/layer3_output_split.bin
add_files -tb bin/layer4_output.bin
add_files -tb bin/layer5_output_concat.bin

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
