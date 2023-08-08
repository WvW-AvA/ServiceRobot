#!/bin/bash
cp -p ../res/iclingo ../res/*.lp .
mkdir log
./cserver -td  ../tests/problems1/NT/stage2 -to 5000 -eval ../lib/libasp -log log -mode it
