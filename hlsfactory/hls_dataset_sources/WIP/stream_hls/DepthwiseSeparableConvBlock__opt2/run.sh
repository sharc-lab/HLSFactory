
#!/bin/bash
# options: -t <task> | csim, syn, cosim, pnr, all

# take argument from command line, default is csim
task=csim

# if an argument is provided, then set task to that argument
if [ $# -eq 1 ]; then
  task=$1
fi

# make sure the task is one of the five options
if [ $task != "csim" ] && [ $task != "syn" ] && [ $task != "cosim" ] && [ $task != "pnr" ] && [ $task != "all" ]; then
  echo "Invalid task: $task"
  echo "Usage: ./run.sh [csim|syn|cosim|pnr|all]"
  exit
fi

# command
ml load xilinx/vivado/2023.2
# set env variable
export PRJ_PATH=$PWD
# run base kernel
# srun -c4 --mem=16GB vitis_hls hls_base.tcl DepthwiseSeparableConvBlock $task -l ${task}_base.log
srun -c4 --mem=16GB vitis_hls hls.tcl DepthwiseSeparableConvBlock $task -l $task.log
