open_project project_1
set_top ResNet

add_files top_new.cpp

open_solution sol1
set_part {xcu250-figd2104-2L-e}
create_clock -period 10 -name default

csynth_design
exit
