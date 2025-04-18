AUTOPILOT_ROOT := /tools/software/xilinx/Vitis_HLS/2022.1
ASSEMBLE_SRC_ROOT := .

IFLAG += -I "${AUTOPILOT_ROOT}/include"
IFLAG += -I "${ASSEMBLE_SRC_ROOT}"
IFLAG += -I "/usr/include/x86_64-linux-gnu"

CFLAG += -fPIC -O3 -std=c++11 -mcmodel=large
CC      = g++ 

ALLOUT+= csim.out

all: IFLAG +=  -g -DCSIM_DEBUG
all: $(ALLOUT) 

debug: IFLAG += -g -DCSIM_DEBUG -DPRINT_DEBUG
debug: csim.out

hls_sim: IFLAG += -g
hls_sim: csim.out

hls_debug: IFLAG += -g -DPRINT_DEBUG
hls_debug: csim.out

utils.o:./utils.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
maxpool2D.o:./maxpool2D.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
tiled_maxpool2D.o:./tiled_maxpool2D.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)

IP_DEP+=utils.o
IP_DEP+=maxpool2D.o
IP_DEP+=tiled_maxpool2D.o

main.o:./sim.cpp
	$(CC) $(GCOV)  $(CFLAG)  -I "${ASSEMBLE_SRC_ROOT}" -o $@  -c $^   -MMD $(IFLAG)

csim.out: main.o $(IP_DEP)
	$(CC)  $(GCOV)  $(CFLAG) -MMD $(IFLAG)  -o $@  $^ 

synth:
	vitis_hls script.tcl

clean:
	rm -f -r csim.d 
	rm -f *.out *.gcno *.gcda *.txt *.o *.d
