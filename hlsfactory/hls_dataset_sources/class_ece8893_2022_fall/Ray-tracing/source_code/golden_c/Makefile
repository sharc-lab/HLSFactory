
SHELL := /bin/bash

AUTOPILOT_ROOT := /tools/software/xilinx/Vitis_HLS/2021.2

ASSEMBLE_SRC_ROOT := .
IFLAG += -I "${AUTOPILOT_ROOT}/include"

IFLAG += -D__SIM_FPO__ -D__SIM_OPENCV__ -D__SIM_FFT__ -D__SIM_FIR__ -D__SIM_DDS__ -D__DSP48E1__


# IFLAG += -DDEBUG_FILE_PRINT=1
IFLAG += -g 
CFLAG += -fPIC -O3 #-fsanitize=address
CFLAG += -lm
CFLAG += -std=c++11 -Wno-unused-result


all:
	g++ *.cpp -o ray_trace $(CFLAG) $(IFLAG)

clean:
	rm -f *.o ray_trace
	rm -f *.ppm
	rm -f *.log
	rm -rf *_hls

run:
	time ./ray_trace

synth:
	source /tools/software/xilinx/setup_env.sh
	source /tools/software/xilinx/Vitis_HLS/2021.1/settings64.sh
	alias vitis_hls="/tools/software/xilinx/Vitis_HLS/2021.1/bin/vitis_hls"
	alias vivado="/tools/software/xilinx/Vivado/2021.1/bin/vivado"
	lastyear vitis_hls -f raytrace_synth.tcl