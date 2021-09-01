FILE=hubble.jpg &&
./compile.sh &&
./dc_core -c -f ../examples/$FILE -t 63 -v &&
./dc_core -d -f ../examples/$FILE.dci -v