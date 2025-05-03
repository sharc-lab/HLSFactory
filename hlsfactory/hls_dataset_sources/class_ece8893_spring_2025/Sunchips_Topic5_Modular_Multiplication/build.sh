#!/bin/bash
CC=g++
CFLAG="-std=c++17 -I/tools/software/xilinx/Vitis_HLS/2023.1/include -Wall -O3 "
SRC="test.cpp"
rm 16 32 64 2>/dev/null

$CC $CFLAG -DBIT16 $SRC -o 16 || {
    echo "cannot build 16-bit binary"
    exit 1
}

$CC $CFLAG -DBIT32 $SRC -o 32 || {
    echo "cannot build 32-bit binary"
    exit 1
}
$CC $CFLAG $SRC -o 64 || {
    echo "cannot build 64-bit binary"
    exit 1
}

echo 
timeout -s 9 60 ./16

echo 
timeout -s 9 60 ./32

echo 
timeout -s 9 60 ./64
