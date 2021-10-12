#!/bin/bash

echo "Compile..." && \
gcc main.c \
    dc/dc.c \
    dc/stb/stb_vorbis.c \
    -lm \
    -o dc_core -O && \
echo "Done" || \
echo "Error"