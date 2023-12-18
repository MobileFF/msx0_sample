import serial
import sys
import io
import time
import os
from serPri import println,printDataStatement
import datetime

start = datetime.datetime.today()

# Serial Port Setting
ser = serial.Serial('/dev/ttyACM0',115200,timeout=5)

print("run serbinsv.msx")
println("screen 0",ser)
println("GET TIME T$:?T$",ser)
time.sleep(1)
ser.write("bload".encode())
time.sleep(1)
ser.write('\"'.encode())
time.sleep(0.5)
ser.write("serbinsv.msx".encode())
time.sleep(0.5)
ser.write('\"'.encode())
time.sleep(0.5)
ser.write(','.encode())
time.sleep(0.5)
ser.write('r'.encode())
time.sleep(0.5)
ser.write('\r\n'.encode())
# println("bload\"serbinsv.msx\"\x2Cr",ser)
time.sleep(0.5)

print("data send start.")

# Open File
args = sys.argv
filename = args[1]
file = io.open(filename,"r")

# save File Name
saveFilename = args[2]

println(saveFilename,ser)
time.sleep(0.5)

lineNum=0
# Send to Serial
while True:
  line = file.readline()
  if line=='':
    break
  println(line.strip().replace(' ',''),ser)
  print(line.strip().replace(' ',''))
  lineNum += 1
  if lineNum % 100 == 0:
    print(" {} lines sent. {}".format(lineNum,datetime.datetime.today()))
  time.sleep(0.3)

println(".",ser)
println("GET TIME T$:?T$",ser)
print("data sent complete.")
print(start)
print(datetime.datetime.today())
time.sleep(0.5)
ser.close()
