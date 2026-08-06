open_project mc_gen_rand_prj
set_top genRandNums

add_files genRandNums.cpp
add_files genRandNums.hpp

add_files -tb main.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
