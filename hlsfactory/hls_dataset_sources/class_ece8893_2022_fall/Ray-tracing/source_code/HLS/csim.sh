#!/bin/sh -f

echo "#define CSIM_DEBUG" > config.h

make clean
make all
