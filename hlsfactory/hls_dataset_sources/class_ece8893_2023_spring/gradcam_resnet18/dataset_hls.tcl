open_project gradcam_resnet18_prj
set_top resnet18

add_files resnet18.cpp
add_files conv1/conv1.cpp
add_files conv_ds/conv_ds.cpp
add_files conv_3x3_s1/conv_3x3_s1.cpp
add_files avg_pool/avg_pool.cpp
add_files linear_fc/linear_fc.cpp
add_files resize.cpp
add_files bundles.hpp
add_files util.h
add_files resize.hpp
add_files sim_util.hpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
