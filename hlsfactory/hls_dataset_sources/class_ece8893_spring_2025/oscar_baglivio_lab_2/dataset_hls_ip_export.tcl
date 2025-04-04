open_project oscar_baglivio_lab_2_proj

open_solution "solution1"

# export generated RTL as an IP; can be commented if not needed
export_design -format ip_catalog -flow impl -rtl verilog

exit