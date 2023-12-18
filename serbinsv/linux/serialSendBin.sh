#!/bin/sh
od -tx1 -An -w64 -v $1 > $1.dump
python3 ~/bin/serialSendBin.py $1.dump $1
