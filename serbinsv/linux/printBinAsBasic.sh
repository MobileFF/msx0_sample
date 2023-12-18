#!/bin/sh
od -tx1 -An -w16 -v $1 > $1.dump
python3 ~/bin/printBinAsBasic.py $1.dump $1
