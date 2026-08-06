open_project shibao_su_lab_2_proj
set_top compute_attention_HLS

add_files top.cpp

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 10 -name default

csynth_design
exit
