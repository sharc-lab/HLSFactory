AUTOPILOT_ROOT := /tools/software/xilinx/Vitis_HLS/2022.1
ASSEMBLE_SRC_ROOT := .

IFLAG += -I "${AUTOPILOT_ROOT}/include"
IFLAG += -I "${ASSEMBLE_SRC_ROOT}"
IFLAG += -I "/usr/include/x86_64-linux-gnu"

CFLAG += -fPIC -O3 -std=c++11 -mcmodel=large 
CC      = g++ 

all:
	g++ *.cpp -o main -lm -fopenmp

synth:
	vitis_hls mersenneScript.tcl

clean:
	rm -f -r csim.d 
	rm -f *.out *.gcno *.gcda *.txt *.o *.d