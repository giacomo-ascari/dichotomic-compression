#!/bin/bash

echo "Compile..." && \
g++ ../src/main.cpp \
    ../src/dc/dc.cpp \
    ../src/dc/dc-utils/dc-misc.cpp \
    ../src/dc/dc-utils/dc-pixel.cpp \
    ../src/dc/dc-utils/dc-pixel-matrix.cpp \
    ../src/dc/dc-compressor/dc-compressor.cpp \
    ../src/dc/dc-decompressor/dc-decompressor.cpp \
    -lm \
    -o ../bin/dc_core_l64deb -O -m64 && \
echo "Done" || \
echo "Error"