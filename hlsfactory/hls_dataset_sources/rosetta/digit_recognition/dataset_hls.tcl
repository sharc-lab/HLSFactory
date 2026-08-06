open_project DigitRec_prj
set_top DigitRec

add_files digitrec.cpp -cflags "-I. -std=c++14"

open_solution sol1
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default
config_rtl -reset state

csynth_design
exit
