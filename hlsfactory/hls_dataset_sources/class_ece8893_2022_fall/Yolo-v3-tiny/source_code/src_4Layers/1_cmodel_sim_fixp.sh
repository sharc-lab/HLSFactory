#!/bin/sh -f

echo "#define CMODEL_SIM" > config.h

make clean
make
