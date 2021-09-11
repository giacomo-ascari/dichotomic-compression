#!/bin/bash

timestamp() {
  date +"%s"
}

FOLDER="log/$(timestamp)"
echo $FOLDER
#echo $PWD
../bin/compile.sh
rm -r tmp
mkdir tmp
mkdir log
mkdir $FOLDER
THRS=( 40 50 60 )
for THR in "${THRS[@]}"
do
  echo $THR
  mkdir $FOLDER/$THR
  cp ../examples/* tmp
  for IMAGE in tmp/*
  do
    echo $IMAGE
    ./test.sh $IMAGE $THR
  done
  cp tmp/* $FOLDER/$THR
  rm tmp/*
done
