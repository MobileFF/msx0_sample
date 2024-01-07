from i2c_slave import I2C_slave
import time
from machine import UART,Pin

# I2C設定 (I2C識別ID 0or1, SDA, SCL)
i2c = I2C_slave(0,sda=0,scl=1,slaveAddress=0x3C)

# UART初期設定（UART番号,クロックレート,TXピン：GP4/6Pin,RXピン：GP5/7Pin）
uart = UART(1,baudrate=9600,tx = Pin(4),rx = Pin(5))

while True:
    length = i2c.get()
    need_resp = i2c.get()
    print("length {0}/ need_resp {1}".format(length,need_resp))
    payload = bytearray(length)
    for i in range(length):
        payload[i] = i2c.get()
        print("{0}:{1}".format(i,hex(payload[i])))
    print(payload)
    uart.write(payload)
    uart.flush()
    while (uart.txdone()==False):
        pass
    time.sleep(0.01)
    if need_resp == 1:
        response = bytearray()
        waitCount = 0
        while True:
            resp = uart.read(1)
            if resp == None:
                waitCount += 1
                if waitCount>10000:
                    break
            else:
                response.append(resp[0])
        print(response)
        while i2c.anyRead()==False:
            pass
        for d in response:
            i2c.put(d)
        

            
