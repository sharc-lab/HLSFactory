AUTOPILOT_ROOT := /tools/software/xilinx/Vitis_HLS/2022.1
ASSEMBLE_SRC_ROOT := .

IFLAG += -I "${AUTOPILOT_ROOT}/include"
IFLAG += -I "${ASSEMBLE_SRC_ROOT}"
IFLAG += -I "/usr/include/x86_64-linux-gnu"
IFLAG +=  -g 

CFLAG += -fPIC -O3 -std=c++11 -mcmodel=large -Wconversion
CC      = g++ 

ALLOUT+= csim.out

all: $(ALLOUT) 

debug: IFLAG += -DPRINT_DEBUG
debug: csim.out

##TO BE MODIFIED START

##model_conv.o:./model_conv.cpp
conv1d.o:./conv1d.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)

##TO BE MODIFIED END

#IP_DEP+=model_conv.o
IP_DEP+=conv1d.o

main.o:./sim.cpp
	$(CC) $(GCOV)  $(CFLAG)  -I "${ASSEMBLE_SRC_ROOT}" -o $@  -c $^   -MMD $(IFLAG) 

csim.out: main.o $(IP_DEP)
	$(CC)  $(GCOV)  $(CFLAG) -MMD $(IFLAG)  -o $@  $^ 

clean:
	rm -f -r csim.d 
	rm -f *.out *.gcno *.gcda *.txt *.o *.d

synth:
	vitis_hls script.tcl
