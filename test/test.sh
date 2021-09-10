#!/bin/bash

FILE=$1
THR=$2
../bin/dc_core -c -f $FILE -t $THR -v &&
../bin/dc_core -d -f $FILE.dci -v