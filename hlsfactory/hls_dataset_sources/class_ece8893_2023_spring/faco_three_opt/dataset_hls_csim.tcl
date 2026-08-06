open_project faco_three_opt_prj
set_top three_opt_nn

add_files three_opt_nn.cpp

add_files -tb main.cpp
add_files -tb stats.txt

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
