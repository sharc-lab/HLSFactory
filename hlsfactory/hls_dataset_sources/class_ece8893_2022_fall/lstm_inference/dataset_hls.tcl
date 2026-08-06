open_project lstm_inference_prj
set_top lstmInference

add_files lstmInference.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
