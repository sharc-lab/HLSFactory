open_project TFBuf_wiFW_1_prj
set_top TFBuf_wiFW_1

add_files TFBuf_wiFW_1.cpp -cflags "-Isrc/ -std=c++11"

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 4 -name default

csynth_design
exit
