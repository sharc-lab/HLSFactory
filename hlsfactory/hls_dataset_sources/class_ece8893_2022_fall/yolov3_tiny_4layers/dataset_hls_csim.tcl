open_project yolov3_tiny_4layers_prj
set_top yolov3_tiny

add_files config.h
add_files conv.h
add_files utils.cpp
add_files conv_3x3_id3.cpp
add_files conv_3x3_id16.cpp
add_files conv_ver3.cpp
add_files conv_1x1.cpp
add_files yolov3_tiny.cpp

add_files -tb sim.cpp
add_files -tb bin/bias/bias_10.bin
add_files -tb bin/bias/bias_13.bin
add_files -tb bin/bn/bn_weights_1.bin
add_files -tb bin/bn/bn_weights_11.bin
add_files -tb bin/bn/bn_weights_12.bin
add_files -tb bin/bn/bn_weights_2.bin
add_files -tb bin/bn/bn_weights_3.bin
add_files -tb bin/bn/bn_weights_4.bin
add_files -tb bin/bn/bn_weights_5.bin
add_files -tb bin/bn/bn_weights_6.bin
add_files -tb bin/bn/bn_weights_7.bin
add_files -tb bin/bn/bn_weights_8.bin
add_files -tb bin/bn/bn_weights_9.bin
add_files -tb bin/conv/conv_weights_1.bin
add_files -tb bin/conv/conv_weights_10.bin
add_files -tb bin/conv/conv_weights_11.bin
add_files -tb bin/conv/conv_weights_12.bin
add_files -tb bin/conv/conv_weights_13.bin
add_files -tb bin/conv/conv_weights_2.bin
add_files -tb bin/conv/conv_weights_3.bin
add_files -tb bin/conv/conv_weights_4.bin
add_files -tb bin/conv/conv_weights_5.bin
add_files -tb bin/conv/conv_weights_6.bin
add_files -tb bin/conv/conv_weights_7.bin
add_files -tb bin/conv/conv_weights_8.bin
add_files -tb bin/conv/conv_weights_9.bin
add_files -tb bin/img/input_img_byte.bin
add_files -tb bin/img/input_img_float.bin
add_files -tb bin/yolov3-tiny.weights

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 10 -name default

csim_design
exit
