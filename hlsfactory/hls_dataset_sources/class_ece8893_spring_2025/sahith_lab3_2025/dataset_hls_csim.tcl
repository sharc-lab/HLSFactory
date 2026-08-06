open_project sahith_lab_3_proj
set_top sparse_matrix_multiply_HLS

add_files top.cpp

add_files -tb host.cpp
add_files -tb A_matrix_csr_sparsity_0.10.bin
add_files -tb B_matrix_csc_sparsity_0.10.bin
add_files -tb C_matrix_result_sparsity_0.10.bin
add_files -tb A_matrix_csr_sparsity_0.50.bin
add_files -tb B_matrix_csc_sparsity_0.50.bin
add_files -tb C_matrix_result_sparsity_0.50.bin
add_files -tb A_matrix_csr_sparsity_0.80.bin
add_files -tb B_matrix_csc_sparsity_0.80.bin
add_files -tb C_matrix_result_sparsity_0.80.bin

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 10 -name default

csim_design
exit
