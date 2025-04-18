CXX      = g++
CXXFLAGS_COMMON = -std=c++17 -Wall -Wpedantic -Wextra -fexceptions -fopenmp
IFLAGS = 

AUTOPILOT_ROOT := /tools/software/xilinx/Vitis_HLS/2022.1
ASSEMBLE_SRC_ROOT := ./src

# Change to debug to compile with debugging flags
MODE = release

HOST = $(shell hostname)

ifeq ($(MODE),release)
	CXXFLAGS = $(CXXFLAGS_COMMON) -fPIC -O3 -march=native -flto -mavx2 -DNDEBUG
else
	CXXFLAGS = $(CXXFLAGS_COMMON) -g
endif

# for compatibility with g++ v8 (which is on ECE servers)
ifeq ($(HOST),ece-linlabsrv01.ece.gatech.edu)
	GCCFLAGS = -lstdc++fs
	IFLAGS += -I "${AUTOPILOT_ROOT}/include"
	IFLAGS += -I "${ASSEMBLE_SRC_ROOT}"
	IFLAGS += -I "/usr/include/x86_64-linux-gnu" -MMD -g
else
	GCCFLAGS = 
endif

# IFLAGS += -D__SIM_FPO__ -D__SIM_OPENCV__ -D__SIM_FFT__ -D__SIM_FIR__ -D__SIM_DDS__ -D__DSP48E1__

LDFLAGS  = 

TARGET = faco

BUILDDIR = build

SRCDIR = src

SOURCES = faco.cpp problem_instance.cpp local_search.cpp utils.cpp rand.cpp progargs.cpp

OBJS = $(SOURCES:.cpp=.o)
D_OBJS = $(addprefix $(BUILDDIR)/,$(SOURCES:.cpp=.d))

OUT_OBJS = $(addprefix $(BUILDDIR)/,$(OBJS))

.PHONY: clean all

all: $(TARGET)

$(TARGET): $(OUT_OBJS)
	$(CXX) $(GCCFLAGS) $(CXXFLAGS) $(OUT_OBJS) $(LDFLAGS) -o $(TARGET) $(IFLAGS) $(GCCFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p results
	@mkdir -p $(BUILDDIR)
	$(CXX) $(GCCFLAGS) $(CXXFLAGS) -c $< -o $@ $(IFLAGS) $(GCCFLAGS)

synth:
	vitis_hls script.tcl

clean:
	rm -f $(OUT_OBJS) $(D_OBJS) $(TARGET) stats.txt *.log
	rm -rf aco_ls_proj
