open_project shuffler_in_prj
set_top shuffler_in

add_files shuffler_in.cpp -cflags "-Isrc/ -std=c++11"

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 4 -name default

csynth_design
exit
