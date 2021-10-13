#!/bin/bash

../bin/compile.sh
FILE=$1
THR=$2
../bin/dc_core -c -f $FILE -t $THR -v 2 -V &&
../bin/dc_core -d -f $FILE.dci -V