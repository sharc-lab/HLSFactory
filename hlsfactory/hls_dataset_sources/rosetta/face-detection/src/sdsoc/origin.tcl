open_project -reset test
set_top face_detect
add_files /home/projects/ljohn/simplescalar/zhigang/rosetta/face-detection/src/sdsoc/face_detect_origin.cpp
add_files /home/projects/ljohn/simplescalar/zhigang/rosetta/face-detection/src/sdsoc/face_detect.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_rtl -reset state
csynth_design
close_project


