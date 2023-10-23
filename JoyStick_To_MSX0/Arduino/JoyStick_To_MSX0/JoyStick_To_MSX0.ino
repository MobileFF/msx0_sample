#include "Wire.h"

/*
 * PIN 2 : UP
 * PIN 3 : DOWN
 * PIN 4 : LEFT
 * PIN 5 : RIGHT
 * PIN 6 : TRIGGER 1
 * PIN 7 : TRIGGER 2
 */

int PIN_UP     = 2;
int PIN_DOWN   = 3;
int PIN_LEFT   = 4;
int PIN_RIGHT  = 5;
int PIN_TR1    = 6;
int PIN_TR2    = 7;
int PIN_SELECT = 8;
int PIN_START  = 9;
int PIN_ARY[] = {PIN_UP,PIN_DOWN,PIN_LEFT,PIN_RIGHT,PIN_TR1,PIN_TR2,PIN_SELECT,PIN_START};

byte BIT_UP     = 0b00000001;
byte BIT_DOWN   = 0b00000010;
byte BIT_LEFT   = 0b00000100;
byte BIT_RIGHT  = 0b00001000;
byte BIT_TR1    = 0b00010000;
byte BIT_TR2    = 0b00100000;
byte BIT_SELECT = 0b01000000;
byte BIT_START  = 0b10000000;

byte BIT_ARY[] = {BIT_UP,BIT_DOWN,BIT_LEFT,BIT_RIGHT,BIT_TR1,BIT_TR2,BIT_SELECT,BIT_START};

volatile byte joyStickValue = 0b00000000;;

byte slave_address = 0x08;

void setup() {
  Serial.begin(9600);
  Serial.println("ATARI 9pin JoyStick To MSX0 I2C");

  for(int i=0;i<sizeof(PIN_ARY)/sizeof(PIN_ARY[0]);i++) {
    pinMode(PIN_ARY[i],INPUT_PULLUP);
  }

  Wire.begin(slave_address);
  Wire.onRequest(requestEvent);
  
}

void loop() {
  joyStickValue = 0x00;
  for(int i=0;i<sizeof(PIN_ARY)/sizeof(PIN_ARY[0]);i++) {
    if (digitalRead(PIN_ARY[i])==LOW) {
      joyStickValue |= BIT_ARY[i];  
    }
  }
}

void requestEvent() {
  Wire.write(~joyStickValue);
}
