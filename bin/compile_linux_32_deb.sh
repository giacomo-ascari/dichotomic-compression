#!/bin/bash

echo "Compile..." && \
gcc ../src/main.c \
    ../src/dc/dc.c \
    ../src/dc/stb/stb_vorbis.c \
    -lm \
    -o ../bin/dc_core_l32deb -O -m32 && \
echo "Done" || \
echo "Error"