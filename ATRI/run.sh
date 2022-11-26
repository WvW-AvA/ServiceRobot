#!/bin/bash
cd build 
make -j
cd ../bin 
./atri -nlp 1 -err 0 -ask_2 0 -cons 0
cd ..
