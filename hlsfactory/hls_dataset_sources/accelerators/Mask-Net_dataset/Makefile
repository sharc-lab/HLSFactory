VITIS_HLS_DIR := /tools/software/xilinx/Vitis_HLS/2023.1
BUILD_DIR := .

IFLAG += -I "${BUILD_DIR}"

IFLAG += -I "${VITIS_HLS_DIR}/include"
IFLAG += -I "${VITIS_HLS_DIR}/include/etc"
IFLAG += -I "${VITIS_HLS_DIR}/include/utils"

IFLAG += -I "${VITIS_HLS_DIR}/lnx64/tools/fpo_v7_0"
IFLAG += -I "${VITIS_HLS_DIR}/lnx64/tools/fft_v9_1"
IFLAG += -I "${VITIS_HLS_DIR}/lnx64/tools/fir_v7_0"
IFLAG += -I "${VITIS_HLS_DIR}/lnx64/tools/dds_v6_0"
IFLAG += -I "${VITIS_HLS_DIR}/lnx64/lib/csim"

CFLAG += -L"${VITIS_HLS_DIR}/lnx64/lib/csim" -lhlsmc++-CLANG39 -Wl,-rpath,"${VITIS_HLS_DIR}/lnx64/lib/csim" -Wl,-rpath,"${VITIS_HLS_DIR}/lnx64/tools/fpo_v7_1"
                                            
# for C sim
IFLAG += -D__SIM_FPO__ -D__SIM_OPENCV__ -D__SIM_FFT__ -D__SIM_FIR__ -D__SIM_DDS__ -D__DSP48E1__

IFLAG += -g
CFLAG += -fPIC -O3 -Wno-unused-result
CFLAG += -lm
CFLAG += -std=c++11
# TODO: add support for faster runtime using -ffast-math and -march=native
# CFLAG += -ffast-math -march=native

CXX = ${VITIS_HLS_DIR}/lnx64/tools/clang-3.9/bin/clang++
# TODO: add support for system compilers
# CXX = g++


result: ./*.cc ./*.h
	$(CXX) ${BUILD_DIR}/*.cc -o result $(CFLAG) $(IFLAG)

all: ./result

run: ./result
	./result

clean:
	rm -f *.o result 