#!/bin/bash
cp -p ../res/iclingo ../res/*.lp .
mkdir log
./cserver -td  ../tests/problems/IT/stage1 -to 0 -eval ../lib/libasp -log log
