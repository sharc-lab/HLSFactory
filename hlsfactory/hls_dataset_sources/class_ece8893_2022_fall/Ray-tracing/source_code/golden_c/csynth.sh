#!/bin/sh -f

rm -f config.h
touch config.h

make clean
make synth
