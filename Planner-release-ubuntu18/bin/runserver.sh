#!/bin/bash
cp -p ../res/iclingo ../res/*.lp .
mkdir log
./cserver -td  ../tests/problems/NT/stage1 -to 5000 -eval ../lib/libasp -log log -mode nt
