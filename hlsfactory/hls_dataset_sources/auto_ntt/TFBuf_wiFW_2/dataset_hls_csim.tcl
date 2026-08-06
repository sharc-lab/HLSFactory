open_project TFBuf_wiFW_2_prj
set_top TFBuf_wiFW_2

add_files TFBuf_wiFW_2.cpp -cflags "-Isrc/ -std=c++17 -DHLSFACTORY_CSIM"

add_files -tb host.cpp -cflags "-Isrc/ -std=c++17 -DHLSFACTORY_CSIM"

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 4 -name default

csim_design
exit
