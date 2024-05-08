open_project DGNN_Booster_convLSTM
set_top DGNN_Booster_convLSTM

#add_files ./HLS/dcl.h
add_files ./src/DGNN-booster2-GCRN-M2.cpp
add_files -tb ./src/DGNN-booster2-GCRN-M2.cpp
add_files -tb ./DGNN_parameters/weights/convLSTM.bin
add_files -tb ./DGNN_parameters/convLSTM_uci_bin/node_embedding.bin
add_files -tb ./DGNN_parameters/convLSTM_uci_bin/edge_embedding.bin
add_files -tb ./DGNN_parameters/convLSTM_uci_bin/edge_info.bin
add_files -tb ./DGNN_parameters/convLSTM_uci_bin/edge_num.bin
add_files -tb ./DGNN_parameters/convLSTM_uci_bin/node_num.bin
add_files -tb ./DGNN_parameters/convLSTM_uci_bin/neighborhood_ref_table.bin
add_files -tb ./DGNN_parameters/convLSTM_uci_bin/neighborhood_reverse_ref_table.bin




open_solution "solution3" -flow_target vivado
set_part {xczu9eg-ffvb1156-2-e}
create_clock -period 10 -name default

## C simulation
# Use Makefile instead. This is even slower.
#csim_design -O -clean

## C code synthesis to generate Verilog code
csynth_design

## C and Verilog co-simulation
## This usually takes a long time so it is commented
## You may uncomment it if necessary
##cosim_design

## export synthesized Verilog code
#export_design -format ip_catalog

exit