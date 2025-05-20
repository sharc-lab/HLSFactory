AUTOPILOT_ROOT :=/tools/software/xilinx/Vitis_HLS/2024.1

ASSEMBLE_SRC_ROOT := .

IFLAG += -I "${AUTOPILOT_ROOT}/include"
IFLAG += -D__SIM_FPO__ -D__SIM_OPENCV__ -D__SIM_FFT__ -D__SIM_FIR__ -D__SIM_DDS__ -D__DSP48E1__ -DHLS_NO_XIL_FPO_LIB
IFLAG += -g 

CFLAG += -fPIC -O0
CFLAG += -lm
CFLAG += -std=c++17 -Wno-unused-result 

all:
	g++ -mcmodel=large top.h host.cpp top_new.cpp -o result $(CFLAG) $(IFLAG)
	
	
clean:
	rm -f *.o result
hls:
	vitis-run --mode hls --tcl script.tcl > output_1.log 2>&1
	