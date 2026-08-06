open_project project_1
set_top compute_attention_HLS

add_files top.cpp

add_files -tb host.cpp
add_files -tb Q_tensor.bin
add_files -tb K_tensor.bin
add_files -tb V_tensor.bin
add_files -tb Output_tensor.bin

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 10 -name default

csim_design
exit
