#/bin/sh
zcc +msx -create-app $1.c mymsx.c -subtype=disk -o $1.bin -DAMALLOC -lm
printBinAsBasic.sh $1.msx > $1.msx.bas
