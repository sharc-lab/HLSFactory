open_project fpga_temp_match_prj
set_top SAD_MATCH

add_files fpga_temp_match.cpp
add_files params.h
add_files fpga_temp_matching.h
add_files -tb template_matching_tb.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
