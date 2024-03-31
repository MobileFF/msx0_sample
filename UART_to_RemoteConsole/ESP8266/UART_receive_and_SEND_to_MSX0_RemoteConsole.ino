#include <SoftwareSerial.h>

char buf[512];
int receive_buf_pos = 0;
int send_buf_pos = 0;
int max_pos = 0;
volatile boolean received = false;

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include "secrets.h"

#define HOSTNAME "webserver"

ESP8266WiFiMulti WiFiMulti;
WiFiClient client;

#ifndef D5
#if defined(ESP8266)
#define D8 (15)
#define D5 (14)
#define D7 (13)
#define D6 (12)
#define RX (3)
#define TX (1)
#elif defined(ESP32)
#define D8 (5)
#define D5 (18)
#define D7 (23)
#define D6 (19)
#define RX (3)
#define TX (1)
#endif
#endif

#define D8 (15)
#define D7 (13)

#define BAUD_RATE 300

EspSoftwareSerial::UART mySerial;

void setup() {
  delay(3000);

  Serial.begin(115200);
  Serial.println("UART receive and SEND to MSX0 RemoteConsole");

  mySerial.begin(BAUD_RATE, EspSoftwareSerial::SWSERIAL_8N1, D7, D8, false);
  Serial.println("BAUD_RATE:" + String(BAUD_RATE));
  Serial.println("RX:" + String(D7));
  Serial.println("TX:" + String(D8));

  if (!mySerial) {
    Serial.println("Invalid EspSoftwareSerial pin configuration, check config");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("EspSoftwareSerial pin configuration is valid.");
  
  for (int i = 0; i < sizeof(buf); i++) {
    buf[i] = 0xff;
  }

  if (strlen(ssid) == 0) {
    WiFi.begin();
  } else {
    WiFi.begin(ssid, passPhrase);
  }

  WiFi.setHostname(HOSTNAME);

  Serial.printf("Connect to WiFi...\n");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.printf(".");
  }
  Serial.printf("connected.\n");

  client.connect(IPAddress(192, 168, 1, 227), 2223);
}

void eof() {
  max_pos = receive_buf_pos - 1;
  Serial.println("receive_buf_pos = " + String(receive_buf_pos));
  Serial.println("max_pos = " + String(max_pos));
  send_buf_pos = 0;

  while (mySerial.available() > 0) {
    int val = mySerial.read();
    buf[receive_buf_pos] = val;
    receive_buf_pos++;
  }

  for (int i = 0; i < receive_buf_pos - 1; i++) {
    Serial.print("[" + String(buf[i], HEX) + "]");
  }
  Serial.println();

  receive_buf_pos = 0;
  received = true;

  Serial.println("\n----EOF----");
}

void loop() {

  while (mySerial.available() > 0) {
    if (receive_buf_pos == 0 && received == false) {
      Serial.println("---- receive start ----");
      Serial.println("serial buffer is " + String(mySerial.available()) + " bytes.");
    }
    int val = mySerial.read();
    if (val == 0x1A) {
      eof();
      break;
    } else {
      if (val >= 0x20 || val == 0x0d || val == 0x0a) {
        Serial.write(val);
        buf[receive_buf_pos] = val;
        receive_buf_pos++;
        if (receive_buf_pos == sizeof(buf) / sizeof(buf[0])) {
          Serial.println("\n---- buffer full ----");
          eof();
          break;
        }
      }
    }
  }

  if (received) {
    Serial.println("---- Socket Send Start ----");
    WiFi.mode(WIFI_STA);

    if ((WiFiMulti.run() == WL_CONNECTED)) {

      for (int i = 0; i < max_pos; i++) {
        if (buf[i] > 0) {
          client.write(buf[i]);
          Serial.write(buf[i]);
        }
      }
      Serial.println("[" + String(max_pos - 1) + "]");
      Serial.println("---- Socket Send Complete ----");
      received = false;
      delay(500);

      Serial.println("---- Socket Receive And UART Send Start ----");
      boolean sent = false;
      while (client.available() > 0) {
        sent = true;
        int val = client.read();
        mySerial.write(val);
        Serial.write(val);
        Serial.print("[" + String(val, HEX) + "]");
        delay(10);
      }
      if (sent) {
        mySerial.write('<');
        mySerial.write(0x1a);
      }
      Serial.println("\n---- Socket Receive And UART Send Complete ----");

    } else {
      Serial.println("WiFiMulti is not connected...");
    }
  }
  delay(100);
}
