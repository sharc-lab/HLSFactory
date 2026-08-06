open_project mc_paths_prj
set_top optionValue

add_files optionValueAverage.cpp
add_files optionValueAverage.hpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
