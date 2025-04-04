open_project project_1


open_solution "solution_opt_20_7"

# export generated RTL as an IP; can be commented if not needed
export_design -format ip_catalog -flow impl

exit
