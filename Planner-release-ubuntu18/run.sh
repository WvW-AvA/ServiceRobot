#!/bin/bash
cd build 
make 
cd ../bin 
./atri -nlp 0 -err 1
cd ..
