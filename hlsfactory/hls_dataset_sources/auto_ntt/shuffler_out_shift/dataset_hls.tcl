open_project shuffler_out_shift_prj
set_top shuffler_out_shift

add_files shuffler_out_shift.cpp -cflags "-Isrc/ -std=c++11"

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 4 -name default

csynth_design
exit
