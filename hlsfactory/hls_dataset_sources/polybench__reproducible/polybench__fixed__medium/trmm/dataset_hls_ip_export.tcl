open_project hls_prj
open_solution solution

config_export -vivado_impl_strategy Flow_Quick -vivado_synth_strategy Flow_RuntimeOptimized
export_design -flow impl -format ip_catalog -rtl verilog