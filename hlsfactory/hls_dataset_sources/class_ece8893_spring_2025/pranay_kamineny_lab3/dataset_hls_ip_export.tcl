open_project project_1
open_solution "solution1"

# Export synthesized design as an IP and run Vivado implementation flow
export_design -format ip_catalog -flow impl

exit
