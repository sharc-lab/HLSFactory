open_project project_1
open_solution "solution1"
# export generated RTL as an IP; can be commented if not needed
config_export -vivado_impl_strategy Flow_Quick -vivado_synth_strategy Flow_RuntimeOptimized
export_design -flow impl -format ip_catalog -rtl verilog

exit
