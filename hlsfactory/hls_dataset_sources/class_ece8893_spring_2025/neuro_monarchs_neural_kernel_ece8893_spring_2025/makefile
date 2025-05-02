
###############################################################################
# Simple Makefile to build the host test‑bench for the HLS kernel
# ‑ No pragma‑driven unrolling/parallelism required, plain C++ build
###############################################################################

### Xilinx Vitis HLS installation (adjust if different)
AUTOPILOT_ROOT := /tools/software/xilinx/Vitis_HLS/2023.1

### Project directories
ASSEMBLE_SRC_ROOT := .

### Include / compile flags ---------------------------------------------------
IFLAG += -I"$(AUTOPILOT_ROOT)/include"
IFLAG += -D__SIM_FPO__ -D__SIM_OPENCV__ -D__SIM_FFT__ -D__SIM_FIR__ -D__SIM_DDS__ \
         -D__DSP48E1__ -DHLS_NO_XIL_FPO_LIB
IFLAG += -g

CFLAG  += -fPIC -O0 -lm -std=c++11 -Wno-unused-result

### Source files --------------------------------------------------------------
SRCS  := host.cpp \
         top.cpp

OBJS  := $(SRCS:.cpp=.o)
TARGET := neural_host

### Build rules ---------------------------------------------------------------
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "[LD ] $@"
	@g++ -o $@ $(OBJS) $(CFLAG) $(IFLAG)

%.o: %.cpp
	@echo "[CC ] $<"
	@g++ -c $< -o $@ $(CFLAG) $(IFLAG)

clean:
	@echo "[RM ] objects & binary"
	@rm -f $(OBJS) $(TARGET)
