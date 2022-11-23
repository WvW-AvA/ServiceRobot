#!/bin/bash
cd build 
make 
cd ../bin 
./atri -nlp 1 -err 1
cd ..
