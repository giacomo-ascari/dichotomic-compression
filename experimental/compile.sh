#!/bin/bash

echo "Compile..." && \
g++ main.cpp \
    dc/dc.cpp \
    dc/dc-utils/dc-misc.cpp \
    dc/dc-utils/dc-pixel.cpp \
    dc/dc-utils/dc-pixel-matrix.cpp \
    dc/dc-compressor/dc-compressor.cpp \
    dc/dc-decompressor/dc-decompressor.cpp \
    -lm \
    -o dc_core -O && \
echo "Done" || \
echo "Error"