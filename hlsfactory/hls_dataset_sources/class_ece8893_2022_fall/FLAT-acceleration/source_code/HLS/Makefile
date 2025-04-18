AUTOPILOT_ROOT :=/tools/software/xilinx/Vitis_HLS/2021.1/

ASSEMBLE_SRC_ROOT := .
TB_ROOT := .
IFLAG += -I "${AUTOPILOT_ROOT}/include"
IFLAG += -I "${ASSEMBLE_SRC_ROOT}"
IFLAG += -I "${ASSEMBLE_SRC_ROOT}" 
IFLAG += -I "/usr/include/x86_64-linux-gnu"
IFLAG += -D__SIM_FPO__ -D__SIM_OPENCV__ -D__SIM_FFT__ -D__SIM_FIR__ -D__SIM_DDS__ -D__DSP48E1__

IFLAG +=  -g -DHLS_SIM
CFLAG += -fPIC -O3
CC      = g++


ALLOUT+= csim.out

all: $(ALLOUT) 
##TO BE MODIFIED START

flat.o:./flat.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
utils.o:./utils.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
systolic_array.o:./systolic_array.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
# model_conv.o:./model_conv.cpp
# 	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
# tiled_conv.o:./tiled_conv.cpp
# 	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)

##TO BE MODIFIED END

IP_DEP+=flat.o
IP_DEP+=utils.o
IP_DEP+=systolic_array.o
# IP_DEP+=model_conv.o
# IP_DEP+=tiled_conv.o

main.o:./sim.cpp
	$(CC) $(GCOV)  $(CFLAG)  -I "${ASSEMBLE_SRC_ROOT}" -o $@  -c $^   -MMD $(IFLAG)

csim.out: main.o $(IP_DEP)
	$(CC)  $(GCOV)  $(CFLAG) -MMD $(IFLAG)  -o $@  $^ 

clean:
	rm -f -r csim.d 
	rm -f *.out *.gcno *.gcda *.txt *.o *.d
