#!/bin/sh -f

# echo "#define CSIM_DEBUG" > config.h

rm -f config.h
touch config.h

make clean
make all
