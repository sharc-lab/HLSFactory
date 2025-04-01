open_project compute_attention_HLS_design
open_solution "solution2"
config_export -vivado_impl_strategy Flow_Quick -vivado_synth_strategy Flow_RuntimeOptimized
export_design -flow impl -format ip_catalog -rtl verilog
exit
