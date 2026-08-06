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

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 10 -name default

csynth_design
exit
