AUTOPILOT_ROOT := /tools/software/xilinx/Vitis_HLS/2022.1

ASSEMBLE_SRC_ROOT := .
IFLAG += -I "${AUTOPILOT_ROOT}/include"

IFLAG += -D__SIM_FPO__ -D__SIM_OPENCV__ -D__SIM_FFT__ -D__SIM_FIR__ -D__SIM_DDS__ -D__DSP48E1__


# IFLAG += -DDEBUG_FILE_PRINT=1
IFLAG += -g 
CFLAG += -fPIC -O3 #-fsanitize=address
CFLAG += -lm
CFLAG += -std=c++17 -Wno-unused-result
# Changed from 11 to 17


all:
	g++ *.cpp -o result $(CFLAG) $(IFLAG)
# g++ main.cpp two_opt_nn.cpp -o result $(CFLAG) $(IFLAG)

synth:
	vitis_hls synth.tcl

clean:
	rm -f *.o result *.log
