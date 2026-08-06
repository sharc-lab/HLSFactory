open_project input_selector_single_prj
set_top input_selector_single

add_files input_selector_single.cpp -cflags "-Isrc/ -std=c++11"

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 4 -name default

csynth_design
exit
