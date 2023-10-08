#include "Wire.h"
int i2c_slave_addr = 0x08;

#include <SPI.h>

unsigned char buf[256];
unsigned int data_length = 0;
unsigned int bufpos = 0;
boolean command = true;

void setup(){
  
  Serial.begin(115200);
  Serial.println("I2C to SPI(YMF825)");

  for(int i=0;i<256;i++) {
    buf[i] = 0xff;
  }

  delay(500);

  Wire.begin(i2c_slave_addr);
  Wire.onReceive(dataReceive);
  Wire.onRequest(requestEvent);
  
//  SPI.setBitOrder(MSBFIRST);
//  SPI.setClockDivider(SPI_CLOCK_DIV8);
//  SPI.setDataMode(SPI_MODE0);
  SPI.begin();
  digitalWrite(SS,HIGH);

  // YMF825 Reset
  pinMode(9,OUTPUT);
  digitalWrite(9,LOW);
  delay(100);
  digitalWrite(9,HIGH);
  delay(100);  
}

void loop(){
}

// I2C data receive from master
void dataReceive(int number) {
  if (Wire.available()) {
    if (command) {
      data_length = Wire.read();
      command=false;
      Serial.println("length:"+String(data_length));
      Serial.print("(");
    } else {
      buf[bufpos] = Wire.read();
      Serial.print(String(buf[bufpos])+" ");
      bufpos++;
      if (bufpos==data_length) {    
        sendToSPI(buf,data_length);
        command=true;
        bufpos=0;
        Serial.println(")");
      }
    }
  }
}

// Send data to SPI slave
int sendToSPI(unsigned char* val,int number) {
  Serial.print(">"+String(number));
  Serial.print("[");
  for(int i=0;i<number;i++) {
    Serial.print(String(val[i],HEX)+" ");
  }    
  Serial.print("]");
  digitalWrite(SS,LOW);
  SPI.transfer(val,number);
  digitalWrite(SS,HIGH);
//  digitalWrite(SS,LOW);
//  int rx = SPI.transfer(0);
//  digitalWrite(SS,HIGH);
  Serial.println();
//  return rx;
  return 0;
}

// I2C data send to master
void requestEvent() {
}
