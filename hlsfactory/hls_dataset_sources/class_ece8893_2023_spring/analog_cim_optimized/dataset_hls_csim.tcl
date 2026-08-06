open_project analog_cim_optimized_prj
set_top tiled_cim_conv

add_files tiled_cim_conv.cpp
add_files cim_conv.cpp
add_files conv.h
add_files utils.cpp
add_files utils.h

add_files -tb sim.cpp
add_files -tb csv/correct_output.csv
add_files -tb csv/input2d.csv
add_files -tb csv/v_ref.csv
add_files -tb csv/weight2d.csv
add_files -tb csv/weight2d_cond.csv

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
