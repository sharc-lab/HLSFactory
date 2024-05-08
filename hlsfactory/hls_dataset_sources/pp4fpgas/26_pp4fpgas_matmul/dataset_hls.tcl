open_project project
add_files matrixmultiplication.cpp
add_files -tb matrixmultiplication-top.cpp
add_files -tb matrixmultiplication.gold.dat
set_top matrixmul
open_solution solution1
set_part virtex7
create_clock -period 5
csynth_design
