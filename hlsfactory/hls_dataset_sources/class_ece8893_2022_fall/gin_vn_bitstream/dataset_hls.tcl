open_project gin_vn_bitstream_prj
set_top GIN_virtualnode_compute_one_graph

add_files GIN_virtualnode_compute.cpp
add_files dcl.hpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
