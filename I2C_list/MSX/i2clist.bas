1 'SAVE"I2CLIST.BAS"
100 'Init
110   PA$="device/i2c_"
120   P$(0)=PA$+"a"
130   P$(1)=PA$+"i"
200   ' Internal I2C Device Info
210   D$(0)="08":DN$(0)="Faces II KEYBOARD/JOYPAD" :'Faces II attached
220   D$(1)="34":DN$(1)="AXP192"
230   D$(2)="38":DN$(2)="FT6336U"
240   D$(3)="51":DN$(3)="BM8563"
250   D$(4)="68":DN$(4)="MPU6886" :'BatteryBottom II attached

1000 'Find I2C Devices
1100  FOR I=0 TO 1
1110    PRINT "[";P$(I);"]"
1120    _IOTFIND(P$(I),C)
1130    PRINT "-> ";C;"Devices ";
1140    IF C=0 THEN PRINT "(Not Found)":PRINT:NEXT I
1150    PRINT "Found"
1160    _IOTFIND(P$(I),A$(0),C)
1170    PRINT "  -> Slave Address : ";
1180    FOR J=0 TO C-1
1190        PRINT A$(J);" ";
1200    NEXT J
1300    PRINT
1310    IF INSTR(P$(I),"i2c_i")=0 THEN PRINT:NEXT I
1320    BT=-1 : 'Bottom Parts 0=Faces/1=BatteryBottom
1330    FOR K=0 TO C-1
1340      MP = -1
1350      FOR L=0 TO 4
1360        IF A$(K)=D$(L) THEN PRINT "   ["+D$(L)+"] = ["+DN$(L)+"]"
1370        IF A$(K)=D$(0) THEN BT=0
1380        IF A$(K)=D$(4) THEN MP=1:BT=1
1390      NEXT L
1400    NEXT K
1410    IF MP=-1 THEN BT=0
1420    IF BT=0 THEN PRINT "    -> Faces II Attached"
1430    IF BT=1 THEN PRINT "    -> Battery Bottom II Attached"
1440    PRINT
1490  NEXT I

SAVE"I2CLIST.BAS"
