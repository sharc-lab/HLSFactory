AUTOPILOT_ROOT := /tools/software/xilinx/Vitis_HLS/2022.1
ASSEMBLE_SRC_ROOT := .

IFLAG += -I "${AUTOPILOT_ROOT}/include"
IFLAG += -I "${ASSEMBLE_SRC_ROOT}"
IFLAG += -I "/usr/include/x86_64-linux-gnu"

CFLAG += -fPIC -O3 -std=c++11 -mcmodel=large  -Wconversion
CC      = g++ 

ALLOUT+= sim

all: IFLAG +=  -g -DCSIM_DEBUG
all: $(ALLOUT) 

.PHONY: sim
sim : 
	g++ -DCSIM_DEBUG sim.cpp conv1/conv1.cpp conv_ds/conv_ds.cpp \
		conv_3x3_s1/conv_3x3_s1.cpp \
		avg_pool/avg_pool.cpp linear_fc/linear_fc.cpp resize.cpp

synth:
	vitis_hls script.tcl

clean:
	rm -f -r csim.d 
	rm -f *.out *.gcno *.gcda *.txt *.o *.d
