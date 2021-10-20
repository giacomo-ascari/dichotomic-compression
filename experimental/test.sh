#!/bin/bash

FILE=$1
THR=$2
VER=$3
./dc_core -c -f $FILE -t $THR -v $VER -V &&
./dc_core -d -f $FILE.dci -V