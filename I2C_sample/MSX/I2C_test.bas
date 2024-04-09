1 'SAVE"I2C_TEST.BAS"
1000 'Init
1010   D$="08":'I2C Slave Address
1020   _IOTFIND("device/i2c_a",C)
1030   PRINT "IOTFIND:";C
1040   IF C=0 THEN 1200
1050   _IOTFIND("device/i2c_a",A$(0),C)
1060   PRINT "Slave Address : ";
1070   FOR I=0 TO C-1
1080     PRINT A$(I);" ";
1090     IF A$(I)=D$ THEN 1300
1100   NEXT I
1200 'Device not found
1210   PRINT "Device not found..."
1220   END
1300 'Create Device path
1310   N$="device/i2c_a/"+A$(I)
2000 'Main loop
2010   PRINT
2020   PRINT "---- I2C Communication ----"
2030   FOR I=0 TO 9
2040     C$=CHR$(I)
2050     _IOTPUT(N$,C$)
2060     PRINT "SEND:[";ASC(C$);"] ";
2070     _IOTGET(N$,S$)
2080     PRINT "RECV:";
2090     FOR J=1 TO 16
2100       S1$=MID$(S$,J,1)
2110       PRINT "[";HEX$(ASC(S1$));"]";
2120     NEXT J
2130     PRINT
2140   NEXT I
9999   END
SAVE"I2C_TEST.BAS"