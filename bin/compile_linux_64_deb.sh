#!/bin/bash

echo "Compile..." && \
gcc ../src/main.c \
    ../src/dc/dc.c \
    ../src/dc/stb/stb_vorbis.c \
    -lm \
    -o ../bin/dc_core_l64deb -O -m64 && \
echo "Done" || \
echo "Error"