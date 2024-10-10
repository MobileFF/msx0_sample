1 'SAVE"GTHPA.BAS"
2 'Location Temp Humidity Pres Altitude
1000 'Initialize
1100 _KANJI:WIDTH 32:KEYOFF:CLEAR 1000
1200 MAXFILES=3
1300 DIM DA$(20)
1400 BL=0 : 'Blink Flag
1500 ON STOP GOSUB 60000:STOP ON
1600 AD = &H77 ' BMP280 I2C address
1700 SP# = 101325# ' Sea level Pa
1800 ON INTERVAL=3600 GOSUB 21000 : INTERVAL ON :' Write to File at every 60 secs.

2000 'Device Initialize (1) GPS
2100 CALL COMINI("0:8N1NH",9600)
2200 OPEN "COM:" AS #1
2300 PRINT #1,"$PCAS03,5,0,0,0,0,0,0,0*07" 'Receive GGA every 10 sec
2400 C=LOC(1):IF C>0 THEN INPUT$(C,#1)
2500 CLOSE #1
2600 OPEN "COM:" AS #1
2800 N=16

3000 'Device Initialize (2) File
3100 OPEN "GPS_LOG.TXT" FOR APPEND AS #2
3200 PRINT #2,"DateTime,NS,Lat,EW,Lng,Humidity,Temperature,Pressure,Altitude"

4000 'Device Initialize (2) I2C
4100 N0$="device/i2c_a"
4200 BM$=HEX$(AD)
4300 NP$=N0$+"/"+BM$
4400 _IOTFIND(N0$,C)
4500 IF C=0 THEN PRINT "No I2C Device Connected.":END
4600 _IOTFIND(N0$,A$(0),C)
4700 FOR I=0 TO C-1
4800   IF A$(I)=BM$ THEN PRINT "I2C Device Found.":GOTO 5000
4900 NEXT I : PRINT "I2C Device("+BM$+") Not Found.":END

5000 'Device Initialize (3) BMP280
5100 _IOTPUT(NP$, CHR$(&HE0)+CHR$(&HB6))
5200 T=TIME
5300 IF (TIME-T)<12 THEN 5300
5400 _IOTPUT(NP$,CHR$(&HD0))
5500 _IOTGET(NP$,ID$)
5600 IF (ASC(MID$(ID$,1,1)) <> &H58) THEN PRINT "Error:BMP280 not found":END
5700 PRINT "BMP280 found"
5800 _IOTPUT(NP$, CHR$(&HF4)+CHR$(&H57)) ' ctrl_meas register: normal mode, temperature and pressure oversampling x4
5900 _IOTPUT(NP$, CHR$(&HF5)+CHR$(&H14)) ' config register: standby time 500ms, IIR filter coefficient 16

6000 'Read calibration data
6100 _IOTGET(NP$,D1$):_IOTGET(NP$,D2$):_IOTGET(NP$,D3$):D$=D2$+D3$
6200 FOR I=1 TO 3
6300 IF I=1 THEN T#(1) = ASC(MID$(D$,(I-1)*2+1,1))+ASC(MID$(D$,I*2,1))*256:NEXT I
6400 TT%=VAL("&H"+HEX$(ASC(MID$(D$,I*2,1)))+HEX$(ASC(MID$(D$,(I-1)*2+1,1))))
6500 T#(I)=TT%
6600 NEXT I
7000 FOR I=1 TO 9
7100 IF I=1 THEN P#(I) = ASC(MID$(D$,(I-1)*2+1+6,1))+ASC(MID$(D$,I*2+6,1))*256:NEXT I
7200 PP%=VAL("&H"+HEX$(ASC(MID$(D$,I*2+6,1)))+HEX$(ASC(MID$(D$,(I-1)*2+1+6,1))))
7300 P#(I)=PP%
7400 NEXT I
7500 _CLS

10000 'Main loop
10100 C=LOC(1)
10110 K$=INKEY$:IF K$<>"" THEN GOSUB 25000:GOSUB 21000
10120 LOCATE 29,0:PRINT "    ";:LOCATE 29,0:PRINT C;
10130 IF C<N THEN 10100
10200 A$=INPUT$(N,#1)
10300 FOR I=1 TO N
10400   T$=MID$(A$,I,1):IF T$="$" THEN GOSUB 20000:D$=T$ ELSE D$=D$+T$
10500 NEXT I
10600 GOTO 10000 : 'Main loop

20000 'Print and Write Time and Location
20100 IF LEFT$(D$,6)<>"$GNGGA" THEN RETURN
20200 GOSUB 23000 : 'Split string by ,
20300 GET DATE DT$
20350 GET TIME TM$
20400 _IOTGET("device/dht/humidity",HU)
20450 _IOTGET("device/dht/temperature",TM)
20500 GOSUB 25000 : ' Put Date/Time/Location to Display
20600 GOSUB 30000 : ' Get Temp/Pressure/Altitude
20999 RETURN

21000 'Put Date and Time/Location/Temp/Humidity/Pressure/Altitude to file
21100 FI=2
21120 COLOR 2:LOCATE 0,11:PRINT "WRITE";:COLOR 15
21200 PRINT #FI, "20";DT$;" ";TM$;",";
21300 PRINT #FI, DA$(3);",";
21400 D$=DA$(2):GOSUB 24000 : 'Convert
21500 PRINT #FI, USING "###.######, ";R;
21600 PRINT #FI, DA$(5);",";
21700 D$=DA$(4):GOSUB 24000 : 'Convert
21800 PRINT #FI, USING "###.######,";R;
21900 PRINT #FI, HU;",";
22000 PRINT #FI, TM;",";
22100 PRINT #FI, INT(PS# / 2560#) / 10#;",";
22200 PRINT #FI, INT(AL#*100)/100#;
22300 IF K$<>"" THEN PRINT #FI, ",*";
22400 PRINT #FI,"" 
22500 LOCATE 0,11:PRINT "     ";
22600 RETURN

23000 'Split string by ,
23100 J=0:T2$=""
23200 FOR I=1 TO LEN(D$):T1$=MID$(D$,I,1)
23300   IF T1$="," THEN DA$(J)=T2$:T2$="":J=J+1 ELSE T2$=T2$+T1$
23400 NEXT I
23500 DA$(J)=T2$
23600 RETURN

24000 'Convert
24100 V0=VAL(D$):V1=INT(V0/100)
24200 V2=(V0-V1*100)/60
24300 R=V1+V2
24400 RETURN

25000 'Print Display
25100 IF K$<>"" THEN COLOR 6:LOCATE 10, 0: PRINT "** MARKED **";:COLOR 15:K$=""
25200 LOCATE  3, 2 : PRINT "日時 20";DT$;" ";TM$;
25300 LOCATE  4, 4 : PRINT "緯度 ";DA$(3);" ";
25400 D$=DA$(2) : GOSUB 24000 : 'Convert
25500 PRINT USING "###.######";R;
25600 LOCATE  4, 5 : PRINT "経度 ";DA$(5);" ";
25700 D$=DA$(4) : GOSUB 24000 : 'Convert
25800 PRINT USING "###.######";R
25900 LOCATE  8, 7 : PRINT "気温 ";TM;" ℃";
26000 LOCATE  8, 8 : PRINT "湿度 ";HU;" %";
26100 LOCATE  8, 9 : PRINT "気圧 ";INT(PS#/2560#)/10#; " hPa";
26200 LOCATE  8,10 : PRINT "高度 ";INT(AL#*100#)/100#; " m";
26300 LOCATE 10, 0 : PRINT "             ";
27999 RETURN

30000 ' Read raw temperature and pressure data
30100 _IOTPUT(NP$,CHR$(&HF7)):'BMP280_REG_PRESSUREDATA
30200 _IOTGET(NP$,D1$)
30300 PR# = ASC(MID$(D1$,1,1))*65536+ASC(MID$(D1$,2,1))*256+ASC(MID$(D1$,3,1))
30400 PR# = INT(PR# / 16#)
30410 'PRINT "PRESSURE RAW";PR#
30500 TR# = ASC(MID$(D1$,4,1))*65536+ASC(MID$(D1$,5,1))*256+ASC(MID$(D1$,6,1))
30600 TR# = INT(TR# / 16#)
30610 'PRINT "TEMP RAW";TR#

31000 ' Calculate temperature
31100 V1# = INT(((INT(TR# / 8#) - (T#(1) * 2#)) * T#(2)) / 2048#)
31200 V2# = INT(((INT(TR# / 16#) - T#(1)) * (INT(TR# / 16#) - T#(1))) / 4096#)
31300 V2# = INT((V2# * T#(3)) / 16384#)
31310 'PRINT "V1#";V1#
31320 'PRINT "V2#";V2#

31400 TF# = V1# + V2#
31500 TE# = INT((TF# * 5# + 128#) / 256#)
31510 'PRINT "TE#";TE#

32000 ' Calculate pressure
32100 V1# = TF# - 128000#
32200 V2# = V1# * V1# * P#(6)
32300 V2# = V2# + ((V1# * P#(5)) * 131072#)
32400 V2# = V2# + (P#(4) * 1024# * 1024# * 1024# * 32#)
32500 V1# = INT((V1# * V1# * P#(3)) / 256#) + ((V1# * P#(2)) * 4096#)
32600 V1# = INT(((1# * 8388608# * 16777216#) + V1#) * P#(1) / 1048576# / 8192#)
32700 IF V1# = 0 THEN GOTO 34000:' Prevent division by zero
32800 PS# = 1048576# - PR#
32900 PS# = INT((((PS# * 65536# * 32768# ) - V2#) * 3125#) / V1#)
33000 V1# = INT((P#(9) * INT(PS# / 8192#) * INT(PS# / 8192#)) / 8192# / 4096#)
33100 V2# = INT((P#(8) * PS#) / 1024# / 512#)
33110 'PRINT "V1#";V1#
33120 'PRINT "V2#";V2#
33200 PS# = INT((PS# + V1# + V2#) / 256#) + (P#(7) * 16#)
33210 'PRINT "PS#";PS#

34000   ' Calculate altitude (assuming sea-level pressure = 1013.25 hPa)
34100   AL# = ((SP# / INT(PS# / 256#))^0.1903# - 1#)*(TE#/100+273.15)/0.0065
35000   RETURN

60000 _ANK:_IOTINIT:WIDTH 40:END
