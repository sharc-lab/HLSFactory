#!/bin/sh -f

echo "#define CSIM_DEBUG"  > config.h
echo "#define CMODEL_SIM" >> config.h

make clean
make
