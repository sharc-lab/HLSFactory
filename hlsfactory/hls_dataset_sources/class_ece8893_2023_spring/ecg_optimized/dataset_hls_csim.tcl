open_project ecg_optimized_prj
set_top tiled_conv

add_files conv1d.cpp
add_files conv.h

add_files -tb sim.cpp
add_files -tb bin/README.md
add_files -tb bin/acc.bin
add_files -tb bin/conv1.bin
add_files -tb bin/conv1_bias.bin
add_files -tb bin/conv1_weights.bin
add_files -tb bin/conv2.bin
add_files -tb bin/conv2_bias.bin
add_files -tb bin/conv2_weights.bin
add_files -tb bin/conv3.bin
add_files -tb bin/conv3_bias.bin
add_files -tb bin/conv3_weights.bin
add_files -tb bin/conv4.bin
add_files -tb bin/conv4_bias.bin
add_files -tb bin/conv4_weights.bin
add_files -tb bin/dense1.bin
add_files -tb bin/dense1_bias.bin
add_files -tb bin/dense1_weights.bin
add_files -tb bin/dense2.bin
add_files -tb bin/dense2_bias.bin
add_files -tb bin/dense2_weights.bin
add_files -tb bin/max1.bin
add_files -tb bin/max2.bin
add_files -tb bin/max3.bin
add_files -tb bin/max4.bin
add_files -tb bin/max5.bin

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
