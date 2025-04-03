open_project sparse_mult

open_solution "solution1"

# FPGA part and clock configuration
set_part {xczu3eg-sbva484-1-e}

# export and impl
export_design -flow impl

exit
