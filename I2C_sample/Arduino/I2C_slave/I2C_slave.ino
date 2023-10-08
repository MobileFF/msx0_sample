#include "Wire.h"

volatile byte receiveValue = 0;
volatile boolean received = false;
int address = 0x08; // I2Cスレーブのアドレス

void setup() {
  Wire.begin(address);
  Wire.onReceive(dataReceive);
  Wire.onRequest(requestEvent);

  Serial.begin(9600);
  Serial.print("I2C slave. address=");
  Serial.print("[");
  Serial.print(address,HEX);
  Serial.println("]");
}

void loop() {
  if (received) {
    Serial.println(receiveValue);
    received = false;
  }
}

void dataReceive(int number) {
  if(Wire.available()) {
    receiveValue = Wire.read();
    received = true;
  }
}

void requestEvent() {
  Wire.write(receiveValue+1);
}
