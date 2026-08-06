open_project explainable_ml_prj
set_top tiled_conv

add_files tiled_conv.cpp
add_files gradient.h
add_files utils.h
add_files io.h

add_files -tb sim.cpp
add_files -tb cmodel/fp.h
add_files -tb cmodel/bp.h
add_files -tb cmodel/bp_single_class.h
add_files -tb bin/conv_layer1_bias.bin
add_files -tb bin/conv_layer1_weights.bin
add_files -tb bin/conv_layer2_bias.bin
add_files -tb bin/conv_layer2_weights.bin
add_files -tb bin/conv_layer3_bias.bin
add_files -tb bin/conv_layer3_weights.bin
add_files -tb bin/conv_layer4_bias.bin
add_files -tb bin/conv_layer4_weights.bin
add_files -tb bin/fc1_bias.bin
add_files -tb bin/fc1_weights.bin
add_files -tb bin/fc2_bias.bin
add_files -tb bin/fc2_weights.bin
add_files -tb bin/max_pool1_bias.bin
add_files -tb bin/max_pool1_weights.bin
add_files -tb bin/max_pool2_bias.bin
add_files -tb bin/max_pool2_weights.bin
add_files -tb bin/relu1_bias.bin
add_files -tb bin/relu1_weights.bin

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
