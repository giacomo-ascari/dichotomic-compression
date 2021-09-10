#!/bin/bash

timestamp() {
  date +"%s"
}


../bin/compile.sh
FOLDER="log/$(timestamp)"
echo $FOLDER
mkdir tmp
mkdir log
mkdir $FOLDER

THRS={ 40 50 }
for THR in THRS
do
  echo $THR
  mkdir $FOLDER/$THR

  cp -r "../examples" tmp

  for IMAGE in "tmp/*"
  do
    ./test.sh $IMAGE $THR
  done

  cp -r tmp $FOLDER/$THR
  rm -r tmp

done
