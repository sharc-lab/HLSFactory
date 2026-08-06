open_project shuffler_out_shuff_prj
set_top shuffler_out_shuff

add_files shuffler_out_shuff.cpp -cflags "-Isrc/ -std=c++11"

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 4 -name default

csynth_design
exit
