open_project yolov4_tiny_l6_prj
set_top tiled_conv

add_files conv.h
add_files utils.cpp
add_files utils.h
add_files conv_7x7.cpp
add_files tiled_conv.cpp

add_files -tb sim.cpp
add_files -tb bin/conv_layer6_input2.bin
add_files -tb bin/conv_layer6_output.bin
add_files -tb bin/fused_conv6_bn6_bias.bin
add_files -tb bin/fused_conv6_bn6_weights.bin
add_files -tb bin/layer3_output.bin
add_files -tb bin/layer3_output_split.bin
add_files -tb bin/layer5_output_concat.bin
add_files -tb bin/layer6_output_concat.bin
add_files -tb bin/maxpool_output.bin

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
