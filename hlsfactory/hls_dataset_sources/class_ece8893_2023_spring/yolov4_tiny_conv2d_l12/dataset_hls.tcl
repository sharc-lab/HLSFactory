open_project yolov4_tiny_conv2d_l12_prj
set_top yolov4_tiny

add_files conv.h
add_files utils.cpp
add_files yolov4_tiny.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
