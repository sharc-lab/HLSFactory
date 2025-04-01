open_project project_1_optimized
open_solution "solution1"

# export generated RTL as an IP; can be commented if not needed
export_design -format ip_catalog -flow impl

exit
