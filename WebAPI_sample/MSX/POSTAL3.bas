1 'SAVE"POSTAL3.BAS"
1000 'initialize
1010   _KANJI
1020   CLEAR 1000,&HBFFF
1030   MAXFILES=5
1040   NL$=CHR$(13)+CHR$(10)
1100 'load JSON.BIN
1110   BLOAD "JSON.BIN",R
1120   DEFINT A-Z
1130   JS = &HC000 : 'JSON Document 
1140   OF = 0 : 'OFFSET

2000 'user setting
2010   SV$="zipcloud.ibsnet.co.jp"
2020   PA$="msx/me/if/NET0/" : 'path

3000 'connect
3010   _IOTPUT(PA$+ "conf/addr",SV$)
3020   _IOTPUT(PA$+ "conf/port",80)
3030   _IOTPUT(PA$+ "connect",1)

3500 'check connect status
3510   FOR I=0 TO 100:NEXT
3520   _IOTGET(PA$+ "connect",S)
3530   IF S<>1 THEN PRINT "connect fail":GOTO 9000

4000 'input postal code
4010   INPUT "POSTAL CODE=";P$

5000 'create message
5010   SM$(0)="GET /api/search?zipcode="+P$+" HTTP/1.1"+NL$
5020   SM$(1)="Host: "+SV$+NL$
5030   SM$(2)="Content-Type: application/json"+NL$
5040   SM$(3)=""+NL$

6000 'send message
6010   'PRINT NL$+"---- Send Message ----"
6020   I=0
6030   IF SM$(I)="" THEN 7000
6040   'PRINT SM$(I);
6050   _IOTPUT(PA$+ "msg",SM$(I))
6060   I=I+1
6070   GOTO 6030

7000 'receive message -> store in memory
7010   _IOTGET(PA$+"msg",RM$):IF RM$="" THEN 7010
7020   'PRINT NL$+"---- Receive Message ----"
7030   ER=0:FL=0
7040   FOR J=0 TO 100
7050     IF RM$="" THEN 8000 :'ER=ER+1:GOTO 7200
7060     IF ER=2 THEN 8000
7065     'PRINT RM$;
7070     IS=INSTR(RM$,"{")
7080     IF IS<>0 AND FL=0 THEN FL=1:RM$=MID$(RM$,IS)
7090     IF FL=0 THEN 7200
7100     FOR K=1 TO LEN(RM$)
7110       POKE JS+OF,ASC(MID$(RM$,K,1))
7120       OF=OF+1
7130     NEXT K
7200     _IOTGET(PA$+ "msg",RM$)
7210     FOR I=0 TO 100:NEXT I
7300   NEXT J

8000 'Parse JSON Initialize
8010 S=USR(JS)
8020 IF S=0 THEN PRINT "Not a Valid JSON.":GOTO 9000
8030 PRINT "---- Address ----"

8100 'address1
8110 TX$=USR2("&results#0&address1")
8120 GOSUB 60000
8130 PRINT SJ$;

8200 'address2
8210 TX$=USR2("&results#0&address2")
8220 GOSUB 60000
8230 PRINT SJ$;

8300 'address3
8310 TX$=USR2("&results#0&address3")
8320 GOSUB 60000
8330 PRINT SJ$;

9000 'disconnet
9010   _IOTPUT(PA$+ "connect",0)
9020   _IOTINIT()
9999 END

60000 'UTF8 to SJIS subroutine
60010 '(Original Code is Designed by IKATEN san.Modified by MobileFF)
60100 OPEN"utf8sjis.txt" AS #5 LEN=4
60110 FIELD #5,2 AS KJ$
60120 SJ$=""
60200 '----- MAIN LOOP
60210 FOR P=1 TO LEN(TX$)
60220 CK$=MID$(TX$,P,1):CK=ASC(CK$)
60300 '----- 1 byte charactor
60310 IF CK<128 THEN SJ$=SJ$+CK$;:GOTO 60620
60400 '----- 2 bytes charactor
60410 IF (CK AND 224)<>192 THEN 60510 ELSE P=P+1:GOTO 60620
60500 '----- 3 bytes charactor
60510 IF (CK AND 240)<>224 THEN 60610 ELSE P1=(CK AND 15)*16
60520 IF P=255 THEN 60700:P=0
60530 P=P+1:CK=ASC(MID$(TX$,P,1)):P2=(CK AND 60)/4:P3=(CK AND 3)*64
60540 IF P=255 THEN 60700:P=0
60550 P=P+1:CK=ASC(MID$(TX$,P,1)):P4=(CK AND 63)
60560 GET #5,(P1+P2)*256+P3+P4:SJ$=SJ$+KJ$:GOTO 60620
60600 '----- 4 bytes charactor
60610 IF (CK AND 248)=240 THEN P=P+3
60620 NEXT P
60700 '--------- END OF FILE
60710 CLOSE #5
60720 RETURN

SAVE"POSTAL3.BAS"
