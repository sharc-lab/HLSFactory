open_project mc_mersenne_prj
set_top randomGen

add_files mersenneTwister.cpp
add_files mersenneTwister.hpp

add_files -tb main.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
