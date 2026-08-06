open_project SgdLR_prj
set_top SgdLR

add_files sgd.cpp -cflags "-I. -std=c++14"

add_files -tb host.cpp -cflags "-I. -std=c++14"

open_solution sol1
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default

csim_design
exit
