1 'SAVE"MSXOTCLK.BAS"
100 'Init
110   N0$="device/i2c_i"
120   BM$="51"
130   NP$=N0$+"/"+BM$
200   FOR I=0 TO 6
210     READ DW$(I)
220   NEXT I
230   DATA Sun,Mon,Tue,Wed,Thu,Fri,Sat
500   SCREEN 1
1000 'Find I2C Devices
1100   PRINT "BM8563(RTC) ";
1110   _IOTFIND(N0$,C)
1120   IF C=0 THEN PRINT "Not Found.":END
1200   _IOTFIND(N0$,A$(0),C)
1210   FOR I=0 TO C-1
1220     IF A$(I)=BM$ THEN PRINT "Found.":GOTO 2000
1230   NEXT I
1900   PRINT "Not Found.":END
2000 'BM8563 Initialize
2100   _IOTPUT(NP$,CHR$(&H0)+CHR$(&H0))
2110   _IOTPUT(NP$,CHR$(&H1)+CHR$(&H0))
2120   _IOTPUT(NP$,CHR$(&HD)+CHR$(&H0))
2200  CLS
3000 'Get Date and Time From BM8563(RTC)
3100   GET DATE DM$
3110   GET TIME TM$
3200   FOR I=1 TO 7
3210     _IOTPUT(NP$,CHR$(I+1)):_IOTGET(NP$,D$)
3220     BX=ASC(D$)
3230     IF I=1 THEN BX=BX AND &H7F :'Second
3240     IF I=2 THEN BX=BX AND &H7F :'Minute
3250     IF I=3 THEN BX=BX AND &H3F :'Hour
3260     IF I=4 THEN BX=BX AND &H3F :'Date
3270     IF I=5 THEN BX=BX AND &H7 :'Day of Week
3280     IF I=6 THEN CF=BX AND &H80 : BX=BX AND &H1F :'Month
3300     IF CF<>0 THEN CT=2000 ELSE CT=1900
3400     BC$ = RIGHT$("0"+HEX$(BX),2)
3410     B = VAL("&H"+LEFT$(BC$,1))*10+VAL("&H"+RIGHT$(BC$,1))
3420     'IF I=7 THEN B=B+CT
3430     DT(I-1)=B
3450     'PRINT RIGHT$("0"+HEX$(ASC(D$)),2);" ";
3500   NEXT I
3510   'PRINT
4000   'Output MSX Origin time and Current Time
4100   LOCATE 2,7
4110   PRINT "MSX Origin Time is..."
4200   LOCATE 2,8
4210   PRINT USING "##/##/## ##:##:##";DT(6),DT(5),DT(3),DT(2),DT(1),DT(0)
4310   LOCATE 2,11
4320   PRINT "Current Time is..."
4400   LOCATE 2,12
4410   PRINT DM$;" ";TM$
5000   IF INKEY$="" THEN 3000

SAVE"MSXOTCLK.BAS"
