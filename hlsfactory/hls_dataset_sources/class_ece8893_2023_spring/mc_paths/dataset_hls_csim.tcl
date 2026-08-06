open_project mc_paths_prj
set_top optionValue

add_files optionValueAverage.cpp
add_files optionValueAverage.hpp

add_files -tb main.cpp
add_files -tb data100.csv
add_files -tb data1000.csv

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
