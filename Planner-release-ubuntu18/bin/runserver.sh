#!/bin/bash
cp -p ../res/iclingo ../res/*.lp .
mkdir log
./cserver -td ../tests/example -eval ../lib/libasp -log log
