open_project project_1
set_top sparse_matrix_multiply_HLS

add_files top_dense.cpp

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 10 -name default

csynth_design
exit
