open_project dsc_optimized_prj
set_top tiled_conv

add_files tiled_conv.cpp -cflags "-DCSIM_DEBUG"
add_files conv_3x3.cpp -cflags "-DCSIM_DEBUG"
add_files conv.h
add_files utils.cpp -cflags "-DCSIM_DEBUG"
add_files utils.h

add_files -tb sim.cpp -cflags "-DCSIM_DEBUG"
add_files -tb bin/inputs/input.bin
add_files -tb bin/outputs/features_0_0.bin
add_files -tb bin/outputs/features_0_1.bin
add_files -tb bin/outputs/features_0_2.bin
add_files -tb bin/outputs/features_1_conv_0_0.bin
add_files -tb bin/outputs/features_1_conv_0_1.bin
add_files -tb bin/outputs/features_1_conv_0_2.bin
add_files -tb bin/outputs/features_1_conv_1.bin
add_files -tb bin/outputs/features_1_conv_2.bin
add_files -tb bin/outputs/features_2_conv_0_0.bin
add_files -tb bin/outputs/features_2_conv_0_1.bin
add_files -tb bin/outputs/features_2_conv_0_2.bin
add_files -tb bin/outputs/features_2_conv_1_0.bin
add_files -tb bin/outputs/features_2_conv_1_1.bin
add_files -tb bin/outputs/features_2_conv_1_2.bin
add_files -tb bin/outputs/features_2_conv_2.bin
add_files -tb bin/outputs/features_2_conv_3.bin
add_files -tb bin/weights/fused_features_0_conv0_bn0_bias.bin
add_files -tb bin/weights/fused_features_0_conv0_bn0_weights.bin
add_files -tb bin/weights/fused_features_1_conv_0_conv0_bn0_bias.bin
add_files -tb bin/weights/fused_features_1_conv_0_conv0_bn0_weights.bin
add_files -tb bin/weights/fused_features_1_conv_conv1_bn1_bias.bin
add_files -tb bin/weights/fused_features_1_conv_conv1_bn1_weights.bin
add_files -tb bin/weights/fused_features_2_conv_0_conv0_bn0_bias.bin
add_files -tb bin/weights/fused_features_2_conv_0_conv0_bn0_weights.bin
add_files -tb bin/weights/fused_features_2_conv_1_conv0_bn0_bias.bin
add_files -tb bin/weights/fused_features_2_conv_1_conv0_bn0_weights.bin
add_files -tb bin/weights/fused_features_2_conv_conv2_bn2_bias.bin
add_files -tb bin/weights/fused_features_2_conv_conv2_bn2_weights.bin

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
