#!/bin/bash
cd build 
make -j
cd ../bin 
./atri -nlp 1 -err 1 -ask_2 0 -cons 2 -path ../example/words.txt
cd ..
