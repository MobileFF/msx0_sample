def println(line,ser):
    ser.write(line.encode())
    ser.write('\r\n'.encode())

def printDataStatement(lineNum,line,ser):
    ser.write("{} DATA ".format(lineNum).encode())
    println(line,ser)
