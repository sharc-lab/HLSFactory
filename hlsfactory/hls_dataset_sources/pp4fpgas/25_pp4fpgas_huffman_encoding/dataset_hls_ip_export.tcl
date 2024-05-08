open_project project
open_solution solution1

config_export -vivado_impl_strategy Flow_Quick -vivado_synth_strategy Flow_RuntimeOptimized
export_design -flow impl -format ip_catalog -rtl verilog
