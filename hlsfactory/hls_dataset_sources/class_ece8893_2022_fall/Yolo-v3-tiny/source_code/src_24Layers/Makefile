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

conv_3x3_id3.o:./conv_3x3_id3.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
conv_3x3_id16.o:./conv_3x3_id16.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
conv_ver3.o:./conv_ver3.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
conv_1x1.o:./conv_1x1.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
utils.o:./utils.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
model_conv.o:./model_conv.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)
yolov3_tiny.o:./yolov3_tiny.cpp
	$(CC) $(GCOV)  $(CFLAG)  -o $@ -c $^    -MMD $(IFLAG)

##TO BE MODIFIED END

IP_DEP+=conv_3x3_id3.o
IP_DEP+=conv_3x3_id16.o
IP_DEP+=conv_ver3.o
IP_DEP+=conv_1x1.o
IP_DEP+=utils.o
IP_DEP+=model_conv.o
IP_DEP+=yolov3_tiny.o

main.o:./sim.cpp
	$(CC) $(GCOV)  $(CFLAG)  -I "${ASSEMBLE_SRC_ROOT}" -o $@  -c $^   -MMD $(IFLAG)

csim.out: main.o $(IP_DEP)
	$(CC)  $(GCOV)  $(CFLAG) -MMD $(IFLAG)  -o $@  $^ 

clean:
	rm -f -r csim.d 
	rm -f *.out *.gcno *.gcda *.txt *.o *.d
