#!/bin/bash
A=$(stat -c%s "$1")
B=$(stat -c%s "$2")
C=$(( A * 100 / B ))
echo $C