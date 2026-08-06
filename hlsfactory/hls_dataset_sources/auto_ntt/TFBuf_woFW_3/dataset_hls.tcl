open_project TFBuf_woFW_3_prj
set_top TFBuf_woFW_3

add_files TFBuf_woFW_3.cpp -cflags "-Isrc/ -std=c++11"

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 4 -name default

csynth_design
exit
