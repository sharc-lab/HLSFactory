open_project project_1
set_top mod_mul

add_files top.cpp

add_files -tb host.cpp

open_solution sol1
set_part {xcvu19p-fsva3824-2-e}
create_clock -period 10 -name default

csim_design
exit
