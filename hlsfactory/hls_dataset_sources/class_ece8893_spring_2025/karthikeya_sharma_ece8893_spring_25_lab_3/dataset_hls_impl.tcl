open_project karthikeya_sharma_lab-3_design_proj

open_solution "lab3_karthikeya_Sharma_spring_2025"

config_export -vivado_impl_strategy Flow_Quick -vivado_synth_strategy Flow_RuntimeOptimized

export_design -flow impl -format ip_catalog -rtl verilog