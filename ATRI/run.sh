#!/bin/bash
cd build 
make 
cd ../bin 
./atri -nlp 1 -err 1 -ask_2 0 -cons 0
cd ..
