#!/bin/bash

timestamp() {
  date +"%s"
}

LOG_FOLDER="log/$(timestamp)"
../bin/compile.sh
THRS=( 15 31 47 63 79 95 111 127 )
for THR in "${THRS[@]}"
do 
  for EXAMPLE_PATH in ../examples/*
  do
    IMAGE=$(basename "$EXAMPLE_PATH")
    IMAGE_FOLDER="$LOG_FOLDER/$THR/$IMAGE-DIR"
    IMAGE_PATH="$IMAGE_FOLDER/$IMAGE"
    mkdir -p $IMAGE_FOLDER
    cp $EXAMPLE_PATH $IMAGE_PATH
    ./test.sh $IMAGE_PATH $THR
    FS_RAW=$(stat --printf="%s" $IMAGE_PATH)
    FS_DCI=$(stat --printf="%s" $IMAGE_PATH.dci)
    CR=$(( FS_DCI * 100 / FS_RAW ))
    touch "$IMAGE_FOLDER/$CR"
  done
done
