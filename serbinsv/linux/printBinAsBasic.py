import sys
import io
import time
import os
from conPri import println,printDataStatement

basic = io.open("/home/foobar/bin/crebin.bas.txt","r")

# Send to Serial
while True:
    line = basic.readline()
    if line=='':
        break
    println(line.strip())

# Basic Data Statement Line Number
lineNum = 10000

# Open File
args = sys.argv
filename = args[1]
file = io.open(filename,"r")

# save File Name
saveFilename = args[2]

printDataStatement(lineNum,saveFilename)
lineNum += 1

# file size
file_stats = os.stat(saveFilename)
file_size= file_stats.st_size

printDataStatement(lineNum,str(file_size))
lineNum += 1

# Send to Serial
while True:
    line = file.readline()
    if line=='':
        break
    printDataStatement(lineNum,line.strip().replace(' ',','))
    lineNum += 1

println("RUN")
