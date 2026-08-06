open_project flat_attention_prj
set_top FlatDataflow

add_files flat.cpp
add_files flat.h
add_files utils.cpp
add_files systolic_array.cpp
add_files config.h

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
