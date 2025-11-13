# TCL script for HLS IP Export and Implementation - fir
# Generated automatically for HLSFactory

# Open the project and solution
open_project fir_project
open_solution solution1

# Configure export settings
config_export -vivado_impl_strategy Default -vivado_synth_strategy Default

# Export design and run Vivado implementation
export_design -flow impl -format ip_catalog -rtl verilog
